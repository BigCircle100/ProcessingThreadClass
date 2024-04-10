#include <iostream>
#include <filesystem>
#include "vit.hpp"

void worker_decoder(int channel_id,
                    const std::vector<std::string>& image_names, 
                    const std::string& image_dir, 
                    ViT& vit)
{
  int batch_size = vit.get_batch_size();

  std::vector<std::shared_ptr<ViTData>> batch_data;
  for (auto& name: image_names){
    auto image = cv::imread(image_dir+"/"+name);
    batch_data.emplace_back(std::make_shared<ViTData>(ViTData{channel_id, name, image}));

    // make batch
    if (batch_data.size() == batch_size){
      vit.push_data(batch_data);
      batch_data.clear();
    }
  }

}

void worker_recoder(ViT& vit, int total_count){
  int batch_size = vit.get_batch_size();

  int j = 0;
  for (int i = 0; i < total_count/batch_size; i++){
    std::vector<std::shared_ptr<ViTData>> batch_data;
    vit.get_data(batch_data);
    for (auto & data: batch_data){
      if (data->channel_id == 0){
        // save result, assuming there are 1000 pics each channel
        j+=1;
        std::cout << j << "/1000, channel id: " << data->channel_id << ", image name: " << data->image_name << std::endl;
      }
    }

  }
}


int main(){
  
  // config
  int num_decoder = 1;
  int queue_size = 20;
  std::string input_dir = "../test_dataset";


  // processing class with thread
  ViT vit(queue_size);

  std::vector<std::string> image_names;
  for(const auto& entry: std::filesystem::directory_iterator(input_dir)){
    if (entry.is_regular_file()){
      image_names.emplace_back(entry.path().filename().string());
    }
  }


  std::sort(image_names.begin(), image_names.end());
  int total_count = image_names.size() * num_decoder;

  std::vector<std::thread> decoder_threads;
  for (int i = 0; i < num_decoder; i++){
    decoder_threads.emplace_back(worker_decoder, 
                                i, 
                                image_names,
                                input_dir,
                                std::ref(vit));

  }

  std::thread sink_thread(worker_recoder, std::ref(vit), total_count);

  for (auto & decoder: decoder_threads){
    if (decoder.joinable())
      decoder.join();
  }

  if (sink_thread.joinable())
    sink_thread.join();


}
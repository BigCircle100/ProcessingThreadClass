#include "vit.hpp"

ViT::ViT(int queue_size):m_queue_size(queue_size)
{
  m_thread = std::thread(&ViT::worker_vit, this);
}


ViT::~ViT(){
  m_stop = true;
  m_input_not_empty.notify_all();
  m_thread.join();

}

int ViT::push_data(const std::vector<std::shared_ptr<ViTData>>& data){
  std::unique_lock<std::mutex> lock(m_input_mutex);
  m_input_not_full.wait(lock, [this]{return m_input_queue.size() < m_queue_size;});
  m_input_queue.emplace(data);
  lock.unlock();
  m_input_not_empty.notify_one();
  
}

int ViT::get_data(std::vector<std::shared_ptr<ViTData>>& data){
  std::unique_lock<std::mutex> lock(m_output_mutex);
  m_output_not_empty.wait(lock, [this]{return !m_output_queue.empty();});
  data = std::move(m_output_queue.front());
  m_output_queue.pop();
  lock.unlock();
  m_output_not_full.notify_one();
}

int ViT::get_batch_size(){
  return 1;
}

void ViT::worker_vit(){
  while (true){
    std::vector<std::shared_ptr<ViTData>> data;
    {
      std::unique_lock<std::mutex> lock(m_input_mutex);
      m_input_not_empty.wait(lock, [this]{return m_stop || !m_input_queue.empty();});
      
      if (m_stop && m_input_queue.empty()){
        break;
      }
      
      data = std::move(m_input_queue.front());
      m_input_queue.pop();
      lock.unlock();
      m_input_not_full.notify_one();
    }
    process_vit(data);

    {
      std::unique_lock<std::mutex> lock(m_output_mutex);
      m_output_not_full.wait(lock, [this]{return m_output_queue.size() < m_queue_size;});
      m_output_queue.emplace(data);
      lock.unlock();
      m_output_not_empty.notify_one();
    }
  }

}

void ViT::process_vit(std::vector<std::shared_ptr<ViTData>>& data){
// put your processing code here

}

#ifndef VIT_H
#define VIT_H

#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>


#include "opencv2/opencv.hpp"

struct ViTData
{
  int channel_id;
  std::string image_name;
  cv::Mat image;
  int class_id=-1;
};


class ViT{
public:
  ViT(int queue_size);
  ~ViT();

  int push_data(const std::vector<std::shared_ptr<ViTData>>& data);
  int get_data(std::vector<std::shared_ptr<ViTData>>& data);
  int get_batch_size();


// func
private:
  void worker_vit();
  void process_vit(std::vector<std::shared_ptr<ViTData>>& data);


// var
private:
  std::queue<std::vector<std::shared_ptr<ViTData>>> m_input_queue;
  std::queue<std::vector<std::shared_ptr<ViTData>>> m_output_queue;
  int m_queue_size;

  std::mutex m_input_mutex;
  std::mutex m_output_mutex;
  std::condition_variable m_input_not_full;
  std::condition_variable m_input_not_empty;
  std::condition_variable m_output_not_full;
  std::condition_variable m_output_not_empty;

  std::thread m_thread;
  std::atomic<bool> m_stop = false;


};

#endif
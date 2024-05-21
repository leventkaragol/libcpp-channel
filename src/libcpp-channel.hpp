/*

Thread safe message channel library for C++ (17+)
version 1.0.0
https://github.com/leventkaragol/libcpp-channel

If you encounter any issues, please submit a ticket at https://github.com/leventkaragol/libcpp-channel/issues

Copyright (c) 2024 Levent KARAGÖL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef LIBCPP_CHANNEL_HPP
#define LIBCPP_CHANNEL_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <memory>
#include <vector>

namespace lklibs {

    template <typename T>
    class Channel {
    private:
        struct Data {
            std::queue<T> queue_;
            std::mutex mutex_;
            std::condition_variable cond_var_;
        };

        std::shared_ptr<Data> data_;

    public:
        Channel() : data_(std::make_shared<Data>()) {}

        class Producer {
        public:
            explicit Producer(std::shared_ptr<Data> data) : data_(std::move(data)) {}

            void send(T value) {

                std::unique_lock<std::mutex> lock(data_->mutex_);

                data_->queue_.push(std::move(value));

                lock.unlock();

                data_->cond_var_.notify_one();
            }

        private:
            std::shared_ptr<Data> data_;
        };

        class Consumer {
        public:
            explicit Consumer(std::shared_ptr<Data> data) : data_(std::move(data)) {}

            std::optional<T> receive()            {

                std::unique_lock<std::mutex> lock(data_->mutex_);

                data_->cond_var_.wait(lock, [this]() { return !data_->queue_.empty(); });

                if (data_->queue_.empty()) {

                    return std::nullopt; // Spurious wakeup protection
               }

                T value = std::move(data_->queue_.front());

                data_->queue_.pop();

                return value;
            }

        private:
            std::shared_ptr<Data> data_;
        };

        Producer getProducer()
        {
            return Producer(data_);
        }

        Consumer getConsumer()
        {
            return Consumer(data_);
        }
    };
}

#endif // LIBCPP_CHANNEL_HPP

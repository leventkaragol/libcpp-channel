/*

Thread-safe generic message channel library for C++ (17+)
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
#include <unordered_map>

namespace lklibs
{
    /**
     * @brief Thread-safe generic message channel
     *
     * @tparam T
     */
    template <typename T>
    class Channel
    {
    private:
        struct Data
        {
            std::queue<std::shared_ptr<T>> queue_;
            std::mutex mutex_;
            std::condition_variable cond_var_;
            std::unordered_map<int, std::queue<std::shared_ptr<T>>> consumer_queues_;
            int consumer_id_counter = 0;
        };

        std::shared_ptr<Data> data_;

    public:
        Channel() : data_(std::make_shared<Data>())
        {
        }

        /**
         * @brief Producer class to send messages to the channel
         *
         */
        class Producer
        {
        public:
            explicit Producer(std::shared_ptr<Data> data) : data_(std::move(data))
            {
            }

            /**
             * @brief Send message to the channel
             *
             * @param value
             */
            void send(T value)
            {
                auto message = std::make_shared<T>(std::move(value));

                std::unique_lock<std::mutex> lock(data_->mutex_);

                data_->queue_.push(message);

                // Broadcast message to all consumers
                for (auto& [id, q] : data_->consumer_queues_)
                {
                    q.push(message);
                }

                lock.unlock();

                data_->cond_var_.notify_all();
            }

        private:
            std::shared_ptr<Data> data_;
        };

        /**
         * @brief Consumer class to receive messages from the channel
         *
         */
        class Consumer
        {
        public:
            explicit Consumer(std::shared_ptr<Data> data): data_(std::move(data)), consumer_id_(data_->consumer_id_counter++)
            {
                std::unique_lock<std::mutex> lock(data_->mutex_);

                data_->consumer_queues_[consumer_id_] = std::queue<std::shared_ptr<T>>();
            }

            ~Consumer()
            {
                std::unique_lock<std::mutex> lock(data_->mutex_);

                data_->consumer_queues_.erase(consumer_id_);
            }

            /**
             * @brief Receive message from the channel
             *
             * @return std::optional<T>
             */
            std::optional<T> receive()
            {
                std::unique_lock<std::mutex> lock(data_->mutex_);

                auto& q = data_->consumer_queues_[consumer_id_];

                data_->cond_var_.wait(lock, [&q]() { return !q.empty(); });

                if (q.empty())
                {
                    return std::nullopt; // Spurious wakeup protection
                }

                auto message = q.front();

                q.pop();

                return *message;
            }

        private:
            std::shared_ptr<Data> data_;
            int consumer_id_;
        };

        /**
         * @brief Get producer object which can be used to send messages
         *
         * @return Producer
         */
        Producer getProducer()
        {
            return Producer(data_);
        }

        /**
         * @brief Get consumer object which can be used to receive messages
         *
         * @return Consumer
         */
        Consumer getConsumer()
        {
            return Consumer(data_);
        }
    };
}

#endif // LIBCPP_CHANNEL_HPP

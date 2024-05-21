#include "libcpp-channel.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>

using namespace lklibs;

void produce_once(Channel<std::string>::Producer producer, const std::string& message)
{
    producer.send(message);
}

void consume_once(Channel<std::string>::Consumer consumer, std::string& out_message)
{
    auto message = consumer.receive();
    if (message.has_value())
    {
        out_message = message.value();
    }
}

void consume_multiple(Channel<std::string>::Consumer consumer, std::vector<std::string>& out_messages, int message_count)
{
    for (int i = 0; i < message_count; ++i)
    {
        auto message = consumer.receive();
        if (message.has_value())
        {
            out_messages.push_back(message.value());
        }
    }
}

TEST(ChannelTest, OneProducerOneConsumer)
{
    Channel<std::string> channel;

    auto producer = channel.getProducer();
    auto consumer = channel.getConsumer();

    std::string message_to_send = "Test Message";

    std::string received_message;

    std::thread producer_thread(produce_once, std::move(producer), message_to_send);
    std::thread consumer_thread(consume_once, std::move(consumer), std::ref(received_message));

    producer_thread.join();
    consumer_thread.join();

    EXPECT_EQ(message_to_send, received_message);
}

TEST(ChannelTest, OneProducerTwoConsumers)
{
    Channel<std::string> channel;

    auto producer = channel.getProducer();
    auto consumer1 = channel.getConsumer();
    auto consumer2 = channel.getConsumer();

    std::string message_to_send = "Test Message";

    std::vector<std::string> received_messages1;
    std::vector<std::string> received_messages2;

    std::thread producer_thread(produce_once, std::move(producer), message_to_send);
    std::thread consumer1_thread(consume_once, std::move(consumer1), std::ref(received_messages1.emplace_back()));
    std::thread consumer2_thread(consume_once, std::move(consumer2), std::ref(received_messages2.emplace_back()));

    producer_thread.join();
    consumer1_thread.join();
    consumer2_thread.join();

    ASSERT_EQ(received_messages1.size(), 1);
    ASSERT_EQ(received_messages2.size(), 1);
    EXPECT_EQ(received_messages1[0], message_to_send);
    EXPECT_EQ(received_messages2[0], message_to_send);
}

TEST(ChannelTest, TwoProducersOneConsumer)
{
    Channel<std::string> channel;

    auto producer1 = channel.getProducer();
    auto producer2 = channel.getProducer();
    auto consumer = channel.getConsumer();

    std::string message_to_send1 = "Test Message 1";
    std::string message_to_send2 = "Test Message 2";

    std::vector<std::string> received_messages;

    std::thread producer_thread1(produce_once, std::move(producer1), message_to_send1);
    std::thread producer_thread2(produce_once, std::move(producer2), message_to_send2);
    std::thread consumer_thread(consume_multiple, std::move(consumer), std::ref(received_messages), 2);

    producer_thread1.join();
    producer_thread2.join();
    consumer_thread.join();

    ASSERT_EQ(received_messages.size(), 2);
    EXPECT_TRUE(std::find(received_messages.begin(), received_messages.end(), message_to_send1) != received_messages.end());
    EXPECT_TRUE(std::find(received_messages.begin(), received_messages.end(), message_to_send2) != received_messages.end());
}

TEST(ChannelTest, TwoProducersTwoConsumers)
{
    Channel<std::string> channel;

    auto producer1 = channel.getProducer();
    auto producer2 = channel.getProducer();
    auto consumer1 = channel.getConsumer();
    auto consumer2 = channel.getConsumer();

    std::string message_to_send1 = "Test Message 1";
    std::string message_to_send2 = "Test Message 2";

    std::vector<std::string> received_messages1;
    std::vector<std::string> received_messages2;

    std::thread producer_thread1(produce_once, std::move(producer1), message_to_send1);
    std::thread producer_thread2(produce_once, std::move(producer2), message_to_send2);
    std::thread consumer1_thread(consume_multiple, std::move(consumer1), std::ref(received_messages1), 2);
    std::thread consumer2_thread(consume_multiple, std::move(consumer2), std::ref(received_messages2), 2);

    producer_thread1.join();
    producer_thread2.join();
    consumer1_thread.join();
    consumer2_thread.join();

    ASSERT_EQ(received_messages1.size(), 2);
    ASSERT_EQ(received_messages2.size(), 2);
    EXPECT_TRUE(std::find(received_messages1.begin(), received_messages1.end(), message_to_send1) != received_messages1.end());
    EXPECT_TRUE(std::find(received_messages1.begin(), received_messages1.end(), message_to_send2) != received_messages1.end());
    EXPECT_TRUE(std::find(received_messages2.begin(), received_messages2.end(), message_to_send1) != received_messages2.end());
    EXPECT_TRUE(std::find(received_messages2.begin(), received_messages2.end(), message_to_send2) != received_messages2.end());
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

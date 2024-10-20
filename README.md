# libcpp-channel

Thread-safe generic message channel library for C++ (17+)

[![linux](https://github.com/leventkaragol/libcpp-channel/actions/workflows/linux.yml/badge.svg)](https://github.com/leventkaragol/libcpp-channel/actions/workflows/linux.yml)
[![windows](https://github.com/leventkaragol/libcpp-channel/actions/workflows/windows.yml/badge.svg)](https://github.com/leventkaragol/libcpp-channel/actions/workflows/windows.yml)

> [!TIP]
> Please read this document before using the library. I know, you don't have time but reading
> this document will save you time. I mean just this file, it's not long at all. Trial and error
> will cost you more time.

# Table of Contents

* [What is the channel used for and why do I need it?](#what-is-the-channel-used-for-and-why-do-i-need-it)
* [How to add it to my project](#how-to-add-it-to-my-project)
* [How to use? (Single Producer & Single Consumer)](#how-to-use-single-producer--single-consumer)
* [How to use? (Single Producer & Multiple Consumers)](#how-to-use-single-producer--multiple-consumers)
* [How to use? (Multiple Producers & Single Consumer)](#how-to-use-multiple-producers--single-consumer)
* [How to use? (Multiple Producers & Multiple Consumers)](#how-to-use-multiple-producers--multiple-consumers)
* [Semantic Versioning](#semantic-versioning)
* [License](#license)
* [Contact](#contact)

## What is the channel used for and why do I need it?

In developed applications, there is often a need to exchange messages between different parts of the code. When these 
code blocks are not directly connected and run on separate threads, managing communication between them becomes 
more challenging, especially if there are multiple producers and consumers of messages. In such cases, channels are 
structures used to facilitate communication between independent blocks of code in a thread-safe manner. While they may 
resemble events in appearance, their operating logic is more akin to message queues.

Here are a few situations where you might need channels:

* **Thread-Safe Data Exchange is Required**: When you need to ensure thread-safe communication between multiple threads
* **Decoupling Producers and Consumers**: Decouple the producers and consumers of data, allowing them to operate independently
* **Managing Multiple Producers and Consumers**: When you have multiple threads producing and consuming data, and you need a mechanism to manage their interactions
* **Implementing FIFO Message Queues**: Ensure that messages are processed in the order they were received
* **Simplifying Synchronization**: Simplify the synchronization logic in your application, avoiding complex locking mechanisms
* **Improving Parallelism**: Distribute tasks across multiple threads, enhancing parallelism and performance
* **Handling Real-Time Data**: Manage the quick and secure transmission of real-time data between threads
* **Designing Scalable Systems**: Build scalable systems where components can be added or removed without affecting other parts of the system

## How to add it to my project?

This is a header only library with no external dependency. So actually, all you need is to add the libcpp-channel.hpp file
in src folder to your project and start using it with #include.

You can find usage examples in the examples folder, also find a sample CMakeLists.txt file content below.

```cmake
cmake_minimum_required(VERSION 3.14)

project(myProject)

add_executable(myProject main.cpp libcpp-channel.hpp)

```

## How to use? (Single Producer & Single Consumer)

As an easiest usage example, sending a message from a single producer and receiving the message 
by a single consumer can be done as follows.

```cpp
#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message to the consumer
        producer.send("Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer)
{
    while (true)
    {
        // Receiving message from the producer
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << "Consumer Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer = channel.getProducer();
    auto consumer = channel.getConsumer();

    // Passing producer object to the first thread
    std::thread produce_thread(::produce, std::move(producer));

    // Passing consumer object to the second thread
    std::thread consume_thread(::consume, std::move(consumer));

    produce_thread.join();
    consume_thread.join();

    return 0;
}
```

## How to use? (Single Producer & Multiple Consumers)

In the example below, the message sent from a single producer is received concurrently by two 
different consumers.

```cpp
#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message to all consumers
        producer.send("Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer, const std::string& name)
{
    while (true)
    {
        // Receiving message from the producer
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << name << " Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer = channel.getProducer();
    auto consumer1 = channel.getConsumer();
    auto consumer2 = channel.getConsumer();

    // Passing producer object to the first thread
    std::thread produce_thread(::produce, std::move(producer));

    // Passing consumer objects to other threads
    std::thread consume1_thread(::consume, std::move(consumer1), "Consumer1");
    std::thread consume2_thread(::consume, std::move(consumer2), "Consumer2");

    produce_thread.join();
    consume1_thread.join();
    consume2_thread.join();

    return 0;
}
```

## How to use? (Multiple Producers & Single Consumer)

As a different use case, here is an example of two different producers sending messages to the 
same channel and a single consumer receiving those messages.

```cpp
#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer, const std::string& name)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message with producer name to the consumer
        producer.send(name + " Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer)
{
    while (true)
    {
        // Receiving message from producers
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << "Consumer Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer1 = channel.getProducer();
    auto producer2 = channel.getProducer();
    auto consumer = channel.getConsumer();

    // Passing producer objects to produce threads
    std::thread produce1_thread(::produce, std::move(producer1), "Producer1");
    std::thread produce2_thread(::produce, std::move(producer2), "Producer2");

    // Passing consumer object to the consume thread
    std::thread consume_thread(::consume, std::move(consumer));

    produce1_thread.join();
    produce2_thread.join();
    consume_thread.join();

    return 0;
}
```

## How to use? (Multiple Producers & Multiple Consumers)

In the example below, two different producers send messages to the same channel independently of 
each other, while two different consumers receive these messages independently and concurrently.

```cpp
#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer, const std::string& name)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message with producer name to all consumers
        producer.send(name + " Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer, const std::string& name)
{
    while (true)
    {
        // Receiving message from producers
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << name << " Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer1 = channel.getProducer();
    auto producer2 = channel.getProducer();
    auto consumer1 = channel.getConsumer();
    auto consumer2 = channel.getConsumer();

    // Passing producer objects to produce threads
    std::thread produce1_thread(::produce, std::move(producer1), "Producer1");
    std::thread produce2_thread(::produce, std::move(producer2), "Producer2");

    // Passing consumer object to the consume threads
    std::thread consume1_thread(::consume, std::move(consumer1), "Consumer1");
    std::thread consume2_thread(::consume, std::move(consumer2), "Consumer2");

    produce1_thread.join();
    produce2_thread.join();
    consume1_thread.join();
    consume2_thread.join();

    return 0;
}
```

## Semantic Versioning

Versioning of the library is done using conventional semantic versioning. Accordingly,
in the versioning made as **MAJOR.MINOR.PATCH**;

**PATCH:** Includes possible Bug&Fixes and improvements. You definitely want to get this.

**MINOR:** Additional functionality added via backwards compatibility. You probably want to
get this, it doesn't hurt.

**MAJOR:** Additional functionality that breaks backwards compatibility. You'll need to know
what's changed before you get it, and you'll probably have to make changes to your own code.
If I publish something like this, I will definitely add the changes required for migration
section to the documentation.

## License

MIT License

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

## Contact

If you have problems regarding the library, please open an
[issue on GitHub](https://github.com/leventkaragol/libcpp-channel/issues/new).
Please describe your request, issue, or question in as much detail as possible
and also include the version of your compiler and operating system, as well as
the version of the library you are using. Before opening a new issue, please
confirm that the topic is not already exists in closed issues.

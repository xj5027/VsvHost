#pragma once
#include <cstdint>
#include <cstddef>



namespace VSVLink {

#define MAX_UDP_SIZE 60000

//////////////////////////////////////////////////////////////////////////
/// �����շ������ݡ�ע���������ڴ����
struct DataBuffer {
    DataBuffer()
        : data(nullptr), size(0) {}
    DataBuffer(void *d, std::size_t s)
        : data((std::byte *)d), size(s) {}
    std::byte *data;
    std::size_t size;
};

//////////////////////////////////////////////////////////////////////////
/// �����޸ĵ�Buffer
struct ConstBuffer {
    ConstBuffer()
        : data(nullptr), size(0) {}
    ConstBuffer(const void *_data, std::size_t _size)
        : data((std::byte *)_data), size(_size) {}
    ConstBuffer(DataBuffer buffer)
        : data(buffer.data), size(buffer.size) {}

    const std::byte *data;
    std::size_t size;
};

//////////////////////////////////////////////////////////////////////////
/// ����ÿ֡�յ������ݵĽṹ��
struct FrameData {
    FrameData(void *data, std::size_t size, std::uint64_t t, double sim_t)
        : buffer{data, size}, time(t), sim_time(sim_t) {}
    FrameData(const DataBuffer &buf, std::uint64_t t, double sim_t)
        : buffer(buf), time(t), sim_time(sim_t) {}
    FrameData()
        : time(0) {}

    DataBuffer buffer;
    std::uint64_t time = 0;
    double sim_time = 0.0;
};

}  // namespace VSVLink
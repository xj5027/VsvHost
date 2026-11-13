#include "CommandList.h"

#include <cstring>
#include <iostream>

namespace VSVLink {
CommandList::CommandList()
    : read_pos(0) {}

CommandList::CommandList(const std::byte *buffer, std::size_t size) {
    SetCommandBuffer({buffer, size});
}

void CommandList::Reset() {
    buf.clear();
}

bool CommandList::Empty() {
    return buf.empty();
}

bool CommandList::ReadAll() {
    return buf.size() == read_pos;
}

void CommandList::Append(const void *cmd, std::size_t cmd_size) {
    const std::size_t write_pos = buf.size();
    buf.resize(write_pos + cmd_size);
    std::memcpy(buf.data() + write_pos, cmd, cmd_size);
    CheckBufSize();
}

void CommandList::Append(CmdType command_type, const std::byte *data, std::size_t size) {
    std::size_t const write_pos = buf.size();
    std::size_t const header_size = sizeof(VSVLink::CmdHeader);
    std::size_t const cmd_size = header_size + size;

    buf.resize(buf.size() + cmd_size);

    VSVLink::CmdHeader *header = reinterpret_cast<VSVLink::CmdHeader *>(buf.data() + write_pos);
    header->cmd_size = (std::uint16_t)cmd_size;
    header->cmd_type = command_type;

    // As stated in the standard, nullptr is not allowed with memcpy, even if size == 0
    if (data != nullptr)
        std::memcpy(buf.data() + write_pos + header_size, data, size);
    CheckBufSize();
}

ConstBuffer CommandList::GetBuffer() const {
    return ConstBuffer(buf.data(), buf.size());
}

void CommandList::SetCommandBuffer(const ConstBuffer &buffer) {
    buf.assign(buffer.data, buffer.data + buffer.size);
}

void CommandList::Append(const CommandList &commands) {
    buf.insert(buf.end(), commands.buf.begin(), commands.buf.end());
    CheckBufSize();
}

void CommandList::CheckBufSize() {
    if (buf.size() > MAX_UDP_SIZE) {
        std::cerr << "CommandList buffer is too big,  buffer size:" << buf.size() << std::endl;
    }
}
}  // namespace VSVLink
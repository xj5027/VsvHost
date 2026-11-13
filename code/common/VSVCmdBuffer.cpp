#include "VSVCmdBuffer.h"

namespace VSVLink {
ConstBuffer VSVCmdBuffer::FindCommand(CmdType cmd_type) const {
    ResetReadPointer();
    VSVLink::CmdType command_type = VSVLink::INVALID;
    ConstBuffer buffer;
    while (std::tie(command_type, buffer) = NextCommand(),
           command_type != VSVLink::INVALID && buffer.size != 0) {
        if (command_type == cmd_type)
            return buffer;
    }
    return ConstBuffer();
}

std::pair<CmdType, ConstBuffer> VSVCmdBuffer::NextCommand() const {
    std::pair<CmdType, ConstBuffer> result = std::make_pair(VSVLink::INVALID, ConstBuffer());

    std::size_t const remaining_size = buffer_.size - read_pos;
    if (remaining_size < sizeof(VSVLink::CmdHeader))
        return result;

    VSVLink::CmdHeader const *header = reinterpret_cast<VSVLink::CmdHeader const *>(buffer_.data + read_pos);
    if (header->cmd_size > remaining_size)
        return result;

    result.first = CmdType(header->cmd_type);
    result.second = ConstBuffer(buffer_.data + read_pos, std::size_t(header->cmd_size));

    read_pos += std::size_t(header->cmd_size);
    return result;
}
}  // namespace VSVLink
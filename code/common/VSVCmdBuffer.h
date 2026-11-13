#pragma once
#include "DataBuffer.h"
#include "CommandList.h"

////////////////////////////////////////////////////////
// 辅助对VSVCmd组成的buffer进行查找、遍历的类，本身不持有或分配内存
namespace VSVLink {
class VSVCmdBuffer {
public:
    VSVCmdBuffer()
        : read_pos(0) {}
    VSVCmdBuffer(const void *data, std::size_t size)
        : buffer_(data, size), read_pos(0) {}
    VSVCmdBuffer(const DataBuffer &buf)
        : buffer_(buf), read_pos(0) {}
    VSVCmdBuffer(const ConstBuffer &buf)
        : buffer_(buf), read_pos(0) {}
    VSVCmdBuffer(const CommandList &cmds)
        : read_pos(0) {
        buffer_ = cmds.GetBuffer();
    }
    void SetBuffer(const std::byte *data, std::size_t size) {
        buffer_.data = data;
        buffer_.size = size;
        read_pos = 0;
    }
    void SetBuffer(const DataBuffer &buffer) {
        buffer_ = buffer;
        read_pos = 0;
    }
    void SetBuffer(const ConstBuffer &buffer) {
        buffer_ = buffer;
        read_pos = 0;
    }
    void SetBuffer(const CommandList &list) {
        buffer_ = list.GetBuffer();
        read_pos = 0;
    }

    const ConstBuffer &GetBuffer() const {
        return buffer_;
    }

    /// \brief 重置命令列表的读指针
    void ResetReadPointer() const {
        read_pos = 0;
    }

    /// \brief 查找并取出指定类型的命令
    ConstBuffer FindCommand(CmdType cmd_type) const;

    /// \brief 从当前的读指针位置解析一个命令，然后移动读指针到下一条命令
    /// \return (command type, data pointer, command size)
    std::pair<CmdType, ConstBuffer> NextCommand() const;

    bool IsEmpty() const {
        return buffer_.size == 0;
    }

protected:
    ConstBuffer buffer_;
    mutable std::size_t read_pos;
};
}  // namespace VSVLink

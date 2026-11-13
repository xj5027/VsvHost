#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>
#include <cstring>
#include "VSV_ICD/VSVProtocolCmdType.h"
#include "DataBuffer.h"
namespace VSVLink {
//////////////////////////////////////////////////////////////////////////
/// ������VSVCmd���д�����࣬�����ʹ��VSVCmdBuffer��
class CommandList {
public:
    CommandList();
    CommandList(const std::byte *buffer, std::size_t size);

    /// \brief ������е������б�
    void Reset();

    bool Empty();

    bool ReadAll();

    /// \brief ��ĩβ����VSVЭ����Ϣ
    template <typename Command>
    void Append(const Command &cmd) {
        const std::size_t write_pos = buf.size();
        buf.resize(write_pos + cmd.header.cmd_size);
        std::memcpy(buf.data() + write_pos, &cmd, cmd.header.cmd_size);
        CheckBufSize();
    }

    /// \brief ��ĩβ����Э����Ϣ
    void Append(const void *cmd, std::size_t cmd_size);
    void Append(const ConstBuffer &buffer) {
        Append(buffer.data, buffer.size);
    }
    /// \brief �������б���ĩβ����һ������(data������header)
    void Append(CmdType cmd_type, const std::byte *data, std::size_t size);

    // ����һ��CommandList���ӵ�ĩβ
    void Append(const CommandList &commands);

    // ��ȡ�ڲ���������ָ��
    ConstBuffer GetBuffer() const;

    // �� buffer �е����ݸ��Ƶ��ڲ���������
    void SetCommandBuffer(const ConstBuffer &buffer);

    void CheckBufSize();

private:
    std::vector<std::byte> buf;
    mutable std::size_t read_pos;
};
}  // namespace VSVLink

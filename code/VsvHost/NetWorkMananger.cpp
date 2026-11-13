#include "Log.h"
#include "enum.h"
#include "NetWorkMananger.h"

#include "NetWorkUav.h"
#include "NetWorkCenter.h"
#include "NetWorkEnviroment.h"
#include "NetWorkWeatherRadar.h"
#include "NetWorkVideoContrl.h"
#include <chrono>
NetWorkManager::NetWorkManager()
{
    if (!ReadConf("../conf/SJ2414.xml") || !InitNetWork())
    {
        Log::Error("Network manager initialization failed");
    }
    std::string device_path = "/dev/input/event3";
    InitKeyEventMonitor(device_path);
    if (fd_ >= 0) {
        InitThread();
    }
}

NetWorkManager::~NetWorkManager()
{
    UnintThread();
    UnintKeyEventMonitor();
}

bool NetWorkManager::ReadConf(std::string joson_file)
{
    map_net_conf_.clear();
    tinyxml2::XMLDocument xml_doc;
    tinyxml2::XMLError ret = xml_doc.LoadFile(joson_file.c_str());
    if (ret != tinyxml2::XML_SUCCESS)
    {
        Log::Error("Load config file failed, error code: %d, path: %s", ret,
                   joson_file.c_str());
        return false;
    }

    tinyxml2::XMLNode *root_node = xml_doc.GetDocument()->FirstChildElement("root");
    if (!root_node)
    {
        Log::Error("Find root node failed, path: %s", joson_file.c_str());
        return false;
    }
    tinyxml2::XMLNode *networks_node = root_node->FirstChildElement("networks");
    if (!networks_node)
    {
        Log::Error("Find element 'networks' failed, path: %s", joson_file.c_str());
        return false;
    }
    else
    {
        tinyxml2::XMLNode *network_node = networks_node->FirstChildElement("network");
        while (network_node)
        {
            tinyxml2::XMLElement *element = dynamic_cast<tinyxml2::XMLElement *>(network_node);
            if (!element)
            {
                Log::Error("ParseNetwork failed");
                return false;
            }

            const char *type_text = element->Attribute("type");
            const char *recv_ip_text = element->Attribute("recv_ip");
            const char *send_ip_text = element->Attribute("send_ip");
            const int recv_port = element->IntAttribute("recv_port");
            const int send_port = element->IntAttribute("send_port");
            if (!type_text || !recv_ip_text || !send_ip_text ||
                recv_port <= 0 || recv_port > 65535 ||
                send_port <= 0 || send_port > 65535)
            {
                Log::Error("Invalid network configuration entry");
                return false;
            }

            const auto type_value = Enum::enum_cast<ENetWorkType>(type_text);
            if (!type_value.has_value())
            {
                Log::Error("Unknown network type: %s", type_text);
                return false;
            }
            const ENetWorkType type = type_value.value();
            if (map_net_conf_.find(type) != map_net_conf_.end())
            {
                Log::Error("Duplicate network type: %s", type_text);
                return false;
            }

            NetConf conf;
            conf.recv_ip = recv_ip_text;
            conf.recv_port = recv_port;
            conf.send_ip = send_ip_text;
            conf.send_port = send_port;
            conf.enabled = element->IntAttribute("enable", 1) != 0;
            map_net_conf_.insert(std::make_pair(type, conf));

            network_node = network_node->NextSiblingElement();
        }
    }
    return true;
}

bool NetWorkManager::InitNetWork()
{
    const auto uav_it = map_net_conf_.find(ENetWorkType::ECT_UAV);
    if (uav_it == map_net_conf_.end())
    {
        Log::Error("Missing required network configuration: ECT_UAV");
        return false;
    }
    if (uav_it->second.enabled)
    {
        const NetConf &conf = uav_it->second;
        const bool is_init = NetWorkUav::GetInstance().InitNetWork(
            conf.recv_ip, conf.recv_port, conf.send_ip, conf.send_port);
        if (!is_init)
        {
            Log::Error("Network initialization failed: ECT_UAV");
            return false;
        }
    }

    const auto environment_it = map_net_conf_.find(ENetWorkType::ECT_ENVIROMENT);
    if (environment_it == map_net_conf_.end())
    {
        Log::Error("Missing required network configuration: ECT_ENVIROMENT");
        return false;
    }
    if (environment_it->second.enabled)
    {
        const NetConf &conf = environment_it->second;
        const bool is_init = NetWorkEnviroment::GetInstance().InitNetWork(
            conf.recv_ip, conf.recv_port, conf.send_ip, conf.send_port);
        if (!is_init)
        {
            Log::Error("Network initialization failed: ECT_ENVIROMENT");
            return false;
        }
    }

    const auto center_it = map_net_conf_.find(ENetWorkType::ECT_ViCenter);
    if (center_it == map_net_conf_.end())
    {
        Log::Error("Missing required network configuration: ECT_ViCenter");
        return false;
    }
    if (center_it->second.enabled)
    {
        const NetConf &conf = center_it->second;
        const bool is_init = NetWorkCenter::GetInstance().InitNetWork(
            conf.recv_ip, conf.recv_port, conf.send_ip, conf.send_port);
        if (!is_init)
        {
            Log::Error("Network initialization failed: ECT_ViCenter");
            return false;
        }
        NetWorkCenter::GetInstance().Init();
    }

    const auto radar_it = map_net_conf_.find(ENetWorkType::ECT_WEATHERRADAR);
    if (radar_it == map_net_conf_.end())
    {
        Log::Error("Missing required network configuration: ECT_WEATHERRADAR");
        return false;
    }
    if (radar_it->second.enabled)
    {
        const NetConf &conf = radar_it->second;
        const bool is_init = NetWorkWeatherRadar::GetInstance().InitNetWork(
            conf.recv_ip, conf.recv_port, conf.send_ip, conf.send_port);
        if (!is_init)
        {
            Log::Error("Network initialization failed: ECT_WEATHERRADAR");
            return false;
        }
    }

    const auto video_it = map_net_conf_.find(ENetWorkType::ECT_VIDEOCONTRL);
    if (video_it != map_net_conf_.end() && video_it->second.enabled)
    {
        const NetConf &conf = video_it->second;
        const bool is_init = NetWorkVideoContrl::GetInstance().InitNetWork(
            conf.recv_ip, conf.recv_port, conf.send_ip, conf.send_port);
        if (!is_init)
        {
            Log::Error("Network initialization failed: ECT_VIDEOCONTRL");
            return false;
        }
    }

    Log::Message("Network initialization complete");
    return true;
}

void NetWorkManager::InitThread()
{
    thread_flg_ = true;
    thread_ = new std::thread(&NetWorkManager::loop, this);
    loop_flg_ = true;
}

void NetWorkManager::UnintThread()
{
    loop_flg_ = false;
    thread_flg_ = false;
    if (thread_ && thread_->joinable())
    {
        thread_->join();
        delete thread_;
        thread_ = nullptr;
    }
}

void NetWorkManager::InitKeyEventMonitor(std::string device_path)
{
    fd_ = open(device_path.c_str(), O_RDONLY);
    if (fd_ < 0) {
        std::cerr << "打开输入设备失败: " << device_path << ", " << strerror(errno) << std::endl;
        return;
    }
    // 获取设备信息
    char name[256] = "Unknown";
    ioctl(fd_, EVIOCGNAME(sizeof(name)), name);
    std::cout << "设备名称: " << name << std::endl;
    std::cout << "开始监听..." << std::endl;
}

void NetWorkManager::UnintKeyEventMonitor()
{
     if (fd_ >= 0) {
            close(fd_);
        }
}

void NetWorkManager::HandleKeyEvent(const input_event& ev)
{
     const char* state = "";
        if (ev.value == 0) {
            state = "释放";
        } else if (ev.value == 1) {
            state = "按下";
        } else if (ev.value == 2) {
            state = "重复";
        }
        
        std::string keyName = GetKeyName(ev.code);
        std::cout << "按键: " << keyName 
                  << " [" << ev.code << "] "
                  << "状态: " << state << std::endl;

        if (ev.value == 1 && keyName.find("1")!= std::string::npos)
        {
            std::cout << u8"前视切换电视" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(1,1);
            // 发送切换红外;
        }
        else if (ev.value == 1 && keyName.find("2")!= std::string::npos)
        {
            std::cout << u8"前视切换红外" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(1,2);
            // 发送前视切换电视
        }else if (ev.value == 1 && keyName.find("3")!= std::string::npos)
        {
            std::cout << u8"左后视切换电视" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(2,1);
            // 发送前视切换红外
        }else if (ev.value == 1 && keyName.find("4")!= std::string::npos)
        {
            std::cout << u8"左后视切换红外" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(2,2);
            // 发送切换电视
        }else if (ev.value == 1 && keyName.find("5")!= std::string::npos)
        {
            std::cout << u8"右后视切换电视" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(3,1);
            // 发送前视切换红外
        }else if (ev.value == 1 && keyName.find("6")!= std::string::npos)
        {
            std::cout << u8"右后视切换红外" << std::endl; 
            NetWorkCenter::GetInstance().change_ir_or_tv(3,2);
            // 发送切换电视
        }
        else if (ev.value == 1 && keyName.find("Q")!= std::string::npos)
        {
            std::cout << u8"创建通用机场 1000" << std::endl; 
            NetWorkCenter::GetInstance().create_custom_airport(1000,88,42,1500,0);
            
        }
         else if (ev.value == 1 && keyName.find("W")!= std::string::npos)
        {
            std::cout << u8"改变通用机场 1000" << std::endl; 
            NetWorkCenter::GetInstance().change_custom_airport(1000,88,42,1500,90);
           
        }
        else if (ev.value == 1 && keyName.find("E")!= std::string::npos)
        {
            std::cout << u8"改变通用机场 1000" << std::endl; 
            NetWorkCenter::GetInstance().change_custom_airport(1000,87,43,1500,0);
           
        }
        
}

std::string NetWorkManager::GetKeyName(int code)
{
    static const char* keyNames[] = {
            "RESERVED", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
            "MINUS", "EQUAL", "BACKSPACE", "TAB", "Q", "W", "E", "R", "T", "Y",
            "U", "I", "O", "P", "LEFTBRACE", "RIGHTBRACE", "ENTER", "LEFTCTRL",
            "A", "S", "D", "F", "G", "H", "J", "K", "L", "SEMICOLON", "APOSTROPHE",
            "GRAVE", "LEFTSHIFT", "BACKSLASH", "Z", "X", "C", "V", "B", "N", "M",
            "COMMA", "DOT", "SLASH", "RIGHTSHIFT", "KPASTERISK", "LEFTALT", "SPACE",
            "CAPSLOCK", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
            "NUMLOCK", "SCROLLLOCK", "KP7", "KP8", "KP9", "KPMINUS", "KP4", "KP5",
            "KP6", "KPPLUS", "KP1", "KP2", "KP3", "KP0", "KPDOT", "ZENKAKUHANKAKU",
            "102ND", "F11", "F12", "RO", "KATAKANA", "HIRAGANA", "HENKAN", "KATAKANAHIRAGANA",
            "MUHENKAN", "KPJPCOMMA", "KPENTER", "RIGHTCTRL", "KPSLASH", "SYSRQ",
            "RIGHTALT", "LINEFEED", "HOME", "UP", "PAGEUP", "LEFT", "RIGHT", "END",
            "DOWN", "PAGEDOWN", "INSERT", "DELETE", "MACRO", "MUTE", "VOLUMEDOWN",
            "VOLUMEUP", "POWER", "KPEQUAL", "KPPLUSMINUS", "PAUSE", "SCALE",
            "KPCOMMA", "HANGEUL", "HANJA", "YEN", "LEFTMETA", "RIGHTMETA", "COMPOSE"
        };
        
        if (code >= 0 && code < 128) {
            return keyNames[code];
        }
        return "UNKNOWN";
}

void NetWorkManager::loop()
{
    struct input_event ev;
    ssize_t bytes;
    while(thread_flg_)
    {
        if (fd_ < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (loop_flg_)
        {
            bytes = read(fd_, &ev, sizeof(struct input_event));
            
            if (bytes < 0) {
                std::cerr << "读取错误: " << strerror(errno) << std::endl;
                 continue;
            }
            
            if (bytes != sizeof(struct input_event)) {
                std::cerr << "读取大小不匹配" << std::endl;
                continue;
            }
            
            if (ev.type == EV_KEY) {
                HandleKeyEvent(ev);
            }
        }
        
    }
}

#pragma once
#include <visibility.h>
#include <defs/MdsInterface.hpp>
#include <defs/MeasurementObjectDefs.hpp>
#include <defs/Receiver.hpp>
#include <set>
#include <mutex>
#include <functional>
#include <string>
#include <vector>
#include <defs/Log.hpp>

#ifdef _WIN32
#ifdef WRITE_API
    class WRITE_API std::thread;
    class WRITE_API std::mutex;
    template class WRITE_API std::vector<double>;
    template class WRITE_API std::vector<char>;
    template class WRITE_API std::vector<std::string>;
    template class WRITE_API std::allocator<std::string>;
    template class WRITE_API std::allocator<char>;
    template class WRITE_API std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
    template class WRITE_API std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >;
    class WRITE_API std::string;
#endif
#endif

namespace visualizer
{
    class WRITE_API WriteFile :
        public MeasurementObject,
        public DataReceiverObject,
        public InterfaceAccess,
        public ObjectControl,
        public GuiControlIfc
    {
        InterfaceAccess* interfaceAccess_;
        uint8_t instanceNb_;
        uint64_t handle_;
        std::string name_;
        MeasurementObjectType type_;
        LoggingInterface *logger_;
        bool isProcessing_;
        bool showGui_;
        std::mutex mtx_;
        std::ofstream arduinoFile_;
        std::ofstream webcamFile_;

    public:
        WriteFile(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
        virtual ~WriteFile();

        //! MeasurementObject interface implementation
        virtual const uint8_t& getInstanceNumber();
        virtual const uint64_t& getHandle();
        virtual const MeasurementObjectType& getType();
        virtual const std::string& getName();

        //! DataReceiverObject interface implementation
        virtual bool validatePackage(DataPackageCPtr package);

        //! InterfaceAccess interface implementation
        virtual void* getInterface(const std::string& interfaceName);

        virtual void initializeObject() override;
        virtual void terminateObject() override;

        void show(ImGuiContext* ctx) override;
        void writeArduinoPackage(DataPackageCPtr pkg);
        void writeWebcamPackage(DataPackageCPtr pkg);
    };
}


extern "C" WRITE_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handleFunc)
{
    return new visualizer::WriteFile(interfaceAccess, instanceNb, name, handleFunc);
}
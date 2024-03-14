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
#include <defs/Distribution.hpp>
#include <queue>

#ifdef _WIN32
#ifdef DUMMY_API
    class DUMMY_API std::thread;
    class DUMMY_API std::mutex;
    template class DUMMY_API std::vector<double>;
    template class DUMMY_API std::vector<DataPackagePtr>;
    template class DUMMY_API std::vector<std::string>;
    template class DUMMY_API std::allocator<std::string>;
    template class DUMMY_API std::allocator<char>;
    template class DUMMY_API std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
    template class DUMMY_API std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >;
    class DUMMY_API std::string;
#endif
#endif

namespace processors
{
    class DUMMY_API RawDataProcessor :
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
        std::vector<DataPackagePtr> packagesBuffer_;
        bool isProcessing_;
        bool showGui_;
        std::mutex mtx_;
        int maxPkgInBuffer_;
        int maxPayloadSize_;

    public:
        RawDataProcessor(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
        virtual ~RawDataProcessor();

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
    };
}


extern "C" DUMMY_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handleFunc)
{
    return new processors::RawDataProcessor(interfaceAccess, instanceNb, name, handleFunc);
}
#pragma once
#include <visibility.h>
#include <defs/MdsInterface.hpp>
#include <defs/Distribution.hpp>
#include <memory>
#include <defs/Distribution.hpp>
#include <defs/Transmitter.hpp>
#include <thread>
#include <mutex>

#ifdef _WIN32
#ifdef RAW_DATA_SOURCE_API
    class RAW_DATA_SOURCE_API std::thread;
    class RAW_DATA_SOURCE_API std::mutex;
    struct RAW_DATA_SOURCE_API DataDistribution;
    template class RAW_DATA_SOURCE_API std::shared_ptr<DataDistribution>;
    template class RAW_DATA_SOURCE_API std::vector<DataPackagePtr>;
    template class RAW_DATA_SOURCE_API std::unique_ptr<std::thread>;
    template class RAW_DATA_SOURCE_API std::allocator<std::string>;
    template class RAW_DATA_SOURCE_API std::allocator<char>;
    template class RAW_DATA_SOURCE_API std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
    template class RAW_DATA_SOURCE_API std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >;
    class RAW_DATA_SOURCE_API std::string;
#endif
#endif

namespace transmitters
{
    class RAW_DATA_SOURCE_API Dummy :
        public InterfaceAccess,
        public MeasurementObject,
        public DataSenderObject,
        public ObjectControl,
        public GuiControlIfc
    {
        std::shared_ptr<DataDistribution> dataDistributinonPtr_;
        InterfaceAccess* interfaceAccess_;
        uint8_t instanceNumber_;
        uint64_t handle_;
        std::string name_;
        MeasurementObjectType type_;
        DataDistribution* dataDistributionInterface_;

    private:
        std::mutex processingMtx_;
        bool isProcessing_;
        bool showGui_;
        int sleepDuration_;
        std::string payload_;
        std::unique_ptr<std::thread> processingThread_;
        void doFSMProcessing();
        
    public:
        Dummy(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
        virtual ~Dummy();
        virtual void* getInterface(const std::string& interfaceName) override;
        virtual void startProcessing() override;
        virtual void endProcessing() override;
        virtual const uint8_t& getInstanceNumber() override;
        virtual const uint64_t& getHandle() override;
        virtual const MeasurementObjectType& getType() override;
        virtual const std::string& getName() override;
        virtual void initializeObject() override;
        virtual void terminateObject() override;
        void show(ImGuiContext* ctx) override;
    };
}

extern "C" RAW_DATA_SOURCE_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle)
{
    return new transmitters::Dummy(interfaceAccess, instanceNb, name, handle);
}
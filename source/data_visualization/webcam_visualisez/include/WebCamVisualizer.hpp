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
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#ifdef WEBCAM_VIS_API
    class WEBCAM_VIS_API std::thread;
    class WEBCAM_VIS_API std::mutex;
    template class WEBCAM_VIS_API std::vector<double>;
    template class WEBCAM_VIS_API std::vector<char>;
    template class WEBCAM_VIS_API std::vector<std::string>;
    template class WEBCAM_VIS_API std::allocator<std::string>;
    template class WEBCAM_VIS_API std::allocator<char>;
    template class WEBCAM_VIS_API std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
    template class WEBCAM_VIS_API std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >;
    class WEBCAM_VIS_API std::string;
#endif
#endif

namespace visualizer
{
    class WEBCAM_VIS_API WebcamVisualizer :
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
        bool showGui_;
        std::mutex mtx_;
        int maxPkgInBuffer_;
        int maxPayloadSize_;

    public:
        WebcamVisualizer(InterfaceAccess* interfaceAccess, uint8_t nb, const std::string& name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handle);
        virtual ~WebcamVisualizer();

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


extern "C" WEBCAM_VIS_API MeasurementObjectPtr createMO(InterfaceAccess* interfaceAccess, const uint8_t instanceNb, const char* name, std::function<uint64_t(uint8_t, MeasurementObjectType)> handleFunc)
{
    return new visualizer::WebcamVisualizer(interfaceAccess, instanceNb, name, handleFunc);
}
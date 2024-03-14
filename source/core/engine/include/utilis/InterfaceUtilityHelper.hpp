#pragma once
#include <defs/MdsInterface.hpp>

namespace core
{
    namespace utility
    {
        /*!
        *   @brief Interface utility helper class.
        */
        class InterfaceUtilityHelper
        {
        private:
            InterfaceAccess* interfaceAccessPtr_; //!< interface access pointer
        public:
            /*!
            *   @brief Interface utility helper constructor.
            *   @param ifc interface access pointer
            */
            InterfaceUtilityHelper(InterfaceAccess* ifc):
                interfaceAccessPtr_(ifc)
            {
            }

            /*!
            *   @brief Method used to retreive a template interface. A wrapper and upgrade over the getInterface method.
            *   @tparam ifc Interface type.
            *   @return Return a pointer of the requested interface.
            */
            template <typename ifc> ifc* getInteface()
            {
                return static_cast<ifc*>(interfaceAccessPtr_->getInterface(typeid(ifc).name()));
            }
        };
    }
}
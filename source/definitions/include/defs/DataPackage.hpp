#pragma once
#include <cstdint>
#include <memory>
#include <cstring>
#include <chrono>

enum class PackageType : uint8_t
{
    generic = 0x00,
    c_struct = 0x01,
    development_package = 0x02,
    arduino=0x04,
    webcam=0x08
};

/*!
*   @brief Data package definition
*/
struct DataPackage
{
    DataPackage()
    {
        type = PackageType::generic;
        size = 0;
        sourceHandle = (uint64_t)-1;
        payload = nullptr;
        timestamp = (uint64_t)-1;

    }

     DataPackage(void* cStruct, size_t structSize,PackageType typeP, bool copy = false, uint64_t handle = 0)
    {
        size = structSize;
        type = typeP;
        sourceHandle = handle;
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if (copy)
        {
            payload = new uint8_t[structSize];
            
            std::memcpy(payload, cStruct, size);
        }
        else
        {
            payload = cStruct;
        }
    }
    
    DataPackage(void* cStruct, size_t structSize, bool copy = false, uint64_t handle = 0)
    {
        size = structSize;
        type = PackageType::c_struct;
        sourceHandle = handle;
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if (copy)
        {
            payload = new uint8_t[structSize];
            
            std::memcpy(payload, cStruct, size);
        }
        else
        {
            payload = cStruct;
        }
    }
    
   
    /*!
    *   @brief Data package destructor. Delete the payload if it exists using the delete[] operator.
    */
    ~DataPackage()
    {
        if (payload)
        {
            delete[] reinterpret_cast<char*>(payload);
        }
    }

    /*!
    *   @brief Data package equal operator.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the two packages are equal, false otherwise.
    */
    friend bool operator == (const DataPackage& lhs, const DataPackage& rhs)
    {
        return lhs.timestamp == rhs.timestamp &&
            lhs.sourceHandle == rhs.sourceHandle &&
            lhs.size == rhs.size &&
            lhs.type == rhs.type &&
            std::memcmp(lhs.payload, rhs.payload, lhs.size) == 0;
    }

    /*!
    *   @brief Data package not equal operator.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the two packages are not equal, false otherwise.
    */
    friend bool operator != (const DataPackage& lhs, const DataPackage& rhs)
    {
        return !(lhs == rhs);
    }

    /*!
    *   @brief Data package less than operator.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the left hand side package is less than the right hand side package, false otherwise.
    */
    friend bool operator < (const DataPackage& lhs, const DataPackage& rhs)
    {
        return lhs.timestamp < rhs.timestamp;
    }

    /*!
    *   @brief Data package greater than operator.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the left hand side package is greater than the right hand side package, false otherwise.
    */
    friend bool operator > (const DataPackage& lhs, const DataPackage& rhs)
    {
        return rhs < lhs;
    }

    /*!
    *   @brief Data package equal operator for smart pointers.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the two packages are equal, false otherwise.
    */
    friend bool operator == (const std::shared_ptr<DataPackage>& lhs, const std::shared_ptr<DataPackage>& rhs)
    {
        return *lhs == *rhs;
    }

    /*!
    *   @brief Data package not equal operator for smart pointers.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the two packages are not equal, false otherwise.
    */
    friend bool operator != (const std::shared_ptr<DataPackage>& lhs, const std::shared_ptr<DataPackage>& rhs)
    {
        return !(lhs == rhs);
    }

    /*!
    *   @brief Data package less than operator for smart pointers.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the left hand side package is less than the right hand side package, false otherwise.
    */
    friend bool operator < (const std::shared_ptr<DataPackage>& lhs, const std::shared_ptr<DataPackage>& rhs)
    {
        return *lhs < *rhs;
    }

    /*!
    *   @brief Data package greater than operator for smart pointers.
    *   @param lhs left hand side of the operator.
    *   @param rhs right hand side of the operator.
    *   @return true if the left hand side package is greater than the right hand side package, false otherwise.
    */
    friend bool operator > (const std::shared_ptr<DataPackage>& lhs, const std::shared_ptr<DataPackage>& rhs)
    {
        return *rhs < *lhs;
    }

    uint64_t timestamp; //!< package timestamp
    uint64_t sourceHandle; //!< package source handle
    size_t size; //!< package size
    PackageType type; //!< package type
    void* payload; //!< pointer to where the package payload starts.
};
//! Data package pointer
using DataPackagePtr = std::shared_ptr<DataPackage>;
//! Const data package pointer
using DataPackageCPtr = const DataPackagePtr;
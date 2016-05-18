#ifndef GRID2DTEST_MEXIO_HPP
#define GRID2DTEST_MEXIO_HPP

#include <matrix.h>
#include <MexMemoryInterfacing/Headers/MexTypeTraits.hpp>
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include "../Headers/Point.hpp"

template <typename T>
inline mxArrayPtr assignmxArray(MexVector<GenericPoint<T>> &PointVectOut) {
    mxClassID ClassID = GetMexType<T>::typeVal;
    mxArrayPtr returnmxArray;
    size_t StructSize[] = {1,1};
    const char * FieldNames[] = {
        "Class",
        "X",
        "Y"
    };
    if (std::is_arithmetic<T>::value){
        returnmxArray = mxCreateStructArray(2, StructSize, 3, FieldNames);

        MexVector<T> XVector(PointVectOut.size());
        MexVector<T> YVector(PointVectOut.size());
        for(int i=0; i<PointVectOut.size(); ++i) {
            XVector[i] = PointVectOut[i].x;
            YVector[i] = PointVectOut[i].y;
        }
        PointVectOut.clear(); PointVectOut.trim();
        auto XmxArray = assignmxArray(XVector);
        auto YmxArray = assignmxArray(YVector);

        mxSetField(returnmxArray, 0, "X", XmxArray);
        mxSetField(returnmxArray, 0, "Y", YmxArray);
        mxSetField(returnmxArray, 0, "ClassName", mxCreateString("PointVector"));
    }
    else{
        returnmxArray = mxCreateNumericMatrix_730(0, 0, ClassID, mxREAL);
    }

    return returnmxArray;
}

template <typename T, class Al>
static void getInputfrommxArray(const mxArray *InputArray, MexVector<GenericPoint<T>, Al> &PointVectIn) {
    /*
     * This function reads the data in InputArray as though it were a valid
     * MexVector<GenericPoint<T>>. Using on a mexvector that hasnt been validated
     * via FieldInfo<MexVector<GenericPoint<T>>>::CheckType is undefined.
     */

    mxArrayPtr XmxArray = mxGetField(InputArray, 0, "X");
    mxArrayPtr YmxArray = mxGetField(InputArray, 0, "Y");

    uint32_t* XData = reinterpret_cast<uint32_t*>(mxGetData(XmxArray));
    uint32_t* YData = reinterpret_cast<uint32_t*>(mxGetData(YmxArray));

    uint32_t NPoints = mxGetNumberOfElements(XmxArray);
    PointVectIn.resize(NPoints);

    for(int i=0; i<NPoints; ++i) {
        PointVectIn[i].x = XData[i];
        PointVectIn[i].y = YData[i];
    }
}

template <
    typename TSpec,
    typename T,
    typename B=typename std::enable_if<std::is_same<T,TSpec>::value>::type,
    class Al>
static int getInputfromStruct(
    const mxArray *InputStruct,
    const char* FieldName,
    MexVector<GenericPoint<T>, Al> &PointVectIn,
    MexMemInputOps InputOps = MexMemInputOps()) {

    const mxArray* StructFieldPtr = getValidStructField<MexVector<GenericPoint<T>, Al>>(InputStruct, FieldName, InputOps);
    if (StructFieldPtr != nullptr) {
        getInputfrommxArray(StructFieldPtr, PointVectIn);
        return 0;
    }
    else {
        return 1;
    }
};

template<typename T, class Al>
class FieldInfo<MexVector<GenericPoint<T>, Al>, void> {
    static inline bool CheckType(const mxArray* InputmxArray) {
        /*
         * Validates if the InputmxArray represents a valid PointVector or not.
         *
         * The validation consists of the following steps:
         *
         * 1.  ClassName == PointVector
         * 2.  X,Y Field must exist.
         * 2.  X,Y == uint32_t vector.
         * 3.  size(X) == size(Y)
         */

        // Validating ClassName
        mxArrayPtr ClassNameField = mxGetField(InputmxArray, 0, "ClassName");
        char* ClassName = mxArrayToString(ClassNameField);
        bool isFieldValidType = !strcasecmp(ClassName, "PointVector");
        mxFree(ClassName);

        // Validating X,Y Members Existance
        mxArrayPtr XmxArray = nullptr;
        mxArrayPtr YmxArray = nullptr;
        if (isFieldValidType) {
            XmxArray = mxGetField(InputmxArray, 0, "X");
            YmxArray = mxGetField(InputmxArray, 0, "Y");

            if (!(XmxArray && YmxArray)) {
                isFieldValidType = false;
            }
        }

        // Validating X,Y Members type
        if (isFieldValidType) {
            if(!(FieldInfo<uint32_t>::CheckType(XmxArray) && FieldInfo<uint32_t>::CheckType(YmxArray))) {
                isFieldValidType = false;
            }
        }

        // Validating X,Y Members Size
        if (isFieldValidType) {
            uint32_t XSize = FieldInfo<uint32_t>::getSize(XmxArray);
            uint32_t YSize = FieldInfo<uint32_t>::getSize(YmxArray);

            if (XSize != YSize) {
                isFieldValidType = false;
            }
        }

        return isFieldValidType;
    }
    static inline uint32_t getSize(const mxArray* InputmxArray) {
        /*
         * This gets the size of the MexVector<GenericPoint<T>> assuming that the
         * InputmxArray represents a valid MexVector<GenericPoint<T>>. If this is not
         * the case the behaviour is undefined.
         */

        mxArrayPtr XmxArray = mxGetField(InputmxArray, 0, "X");
        return FieldInfo<uint32_t>::getSize(XmxArray);
    }
};
#endif //GRID2DTEST_MEXIO_HPP_HPP

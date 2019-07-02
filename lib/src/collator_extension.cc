// cl /c /I"E:\Projects\dart\dart-sdk\sdk\out\ReleaseX64\dart-sdk\include" /I"E:\Projects\cpp\icu4c-64_2-Win64-MSVC2017\include" /DDART_SHARED_LIB /utf-8 collator_extension.cc
// link collator_extension.obj /DLL /SUBSYSTEM:WINDOWS /MACHINE:X64 /LIBPATH:"E:\Projects\dart\dart-sdk\sdk\out\ReleaseX64\dart-sdk\bin" /LIBPATH:"E:\Projects\cpp\icu4c-64_2-Win64-MSVC2017\lib64" dart.lib icuin.lib

// g++ -fPIC -I "/Users/wangjiong/Development/dart/dart-sdk/sdk/xcodebuild/ReleaseX64/dart-sdk/include" -I "/usr/local/Cellar/icu4c/63.1/include" -c collator_extension.cc -arch x86_64 -std=c++11
// gcc -shared -Wl,-install_name,libcollator_extension.dylib,-undefined,dynamic_lookup,-arch,x86_64 -o libcollator_extension.dylib collator_extension.o -L/usr/local/Cellar/icu4c/63.1/lib/ -licuuc -licutu

#include "dart_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unicode/utypes.h>
#include <unicode/ucol.h>
#include <unicode/ustring.h>

Dart_Handle NewDartExceptionWithMessage(const char *library_url,
                                        const char *exception_name,
                                        const char *message)
{
    // Create a Dart Exception object with a message.
    Dart_Handle type =
        Dart_GetType(Dart_LookupLibrary(Dart_NewStringFromCString(library_url)),
                     Dart_NewStringFromCString(exception_name), 0, NULL);

    if (Dart_IsError(type))
    {
        Dart_PropagateError(type);
    }
    if (message != NULL)
    {
        Dart_Handle args[1];
        args[0] = Dart_NewStringFromCString(message);
        if (Dart_IsError(args[0]))
        {
            Dart_PropagateError(args[0]);
        }
        return Dart_New(type, Dart_Null(), 1, args);
    }
    else
    {
        return Dart_New(type, Dart_Null(), 0, NULL);
    }
}

void collator_create_instance(Dart_NativeArguments args)
{
    const char *locale; // args[0]
    UCollator *ucollator;
    Dart_Handle handle;

    handle = Dart_StringToCString(Dart_GetNativeArgument(args, 0), &locale);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);

    UErrorCode status = U_ZERO_ERROR;
    ucollator = ucol_open(locale, &status);

    if (U_FAILURE(status))
    {
        Dart_Handle error = NewDartExceptionWithMessage(
            "dart:core", "Exception", "Unable to create collator instance");
        if (Dart_IsError(error))
            Dart_PropagateError(error);
        Dart_ThrowException(error);
    }

    Dart_Handle retval = Dart_NewInteger((int64_t)ucollator);

    if (Dart_IsError(retval))
        Dart_PropagateError(retval);

    Dart_SetReturnValue(args, retval);
}

void collator_compare(Dart_NativeArguments args)
{
    int64_t instance; // args[0]
    UCollator *ucollator;
    uint16_t *first; // args[1]
    intptr_t first_len = 0;
    uint16_t *second; // args[2]
    intptr_t second_len = 0;
    UCollationResult result = UCOL_EQUAL;
    Dart_Handle handle;

    handle = Dart_IntegerToInt64(Dart_GetNativeArgument(args, 0), &instance);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);
    ucollator = (UCollator *)instance;

    handle = Dart_GetNativeArgument(args, 1);
    Dart_StringLength(handle, &first_len);
    first = reinterpret_cast<uint16_t *>(malloc(first_len * sizeof(uint16_t)));
    handle = Dart_StringToUTF16(handle, first, &first_len);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);

    handle = Dart_GetNativeArgument(args, 2);
    Dart_StringLength(handle, &second_len);
    second = reinterpret_cast<uint16_t *>(malloc(second_len * sizeof(uint16_t)));
    handle = Dart_StringToUTF16(handle, second, &second_len);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);

    result = ucol_strcoll(ucollator, (UChar *)first, first_len, (UChar *)second, second_len);

    free(first);
    free(second);

    Dart_Handle retval = Dart_NewInteger((int64_t)result);
    if (Dart_IsError(retval))
        Dart_PropagateError(retval);

    Dart_SetReturnValue(args, retval);
}

void collator_close(Dart_NativeArguments args)
{
    int64_t instance; // args[0]
    Dart_Handle handle;

    handle = Dart_IntegerToInt64(Dart_GetNativeArgument(args, 0), &instance);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);

    ucol_close((UCollator *)instance);
}

Dart_NativeFunction ResolveName(Dart_Handle name, int argc,
                                bool *auto_setup_scope)
{
    if (!Dart_IsString(name))
        return NULL;
    Dart_NativeFunction result = NULL;
    const char *cname;
    Dart_Handle handle;

    handle = Dart_StringToCString(name, &cname);
    if (Dart_IsError(handle))
        Dart_PropagateError(handle);

    if (strcmp("createInstance", cname) == 0)
        result = collator_create_instance;
    if (strcmp("compare", cname) == 0)
        result = collator_compare;
    if (strcmp("close", cname) == 0)
        result = collator_close;

    return result;
}

DART_EXPORT Dart_Handle collator_extension_Init(Dart_Handle parent_library)
{
    if (Dart_IsError(parent_library))
        return parent_library;

    Dart_Handle result_code =
        Dart_SetNativeResolver(parent_library, ResolveName, NULL);
    if (Dart_IsError(result_code))
        return result_code;

    return Dart_Null();
}
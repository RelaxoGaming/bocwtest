#pragma once


inline std::mutex hook_mutex;
inline std::mutex hook_mutex2;

inline std::recursive_mutex render_hook;
inline uintptr_t orig;
inline uintptr_t orig2;

extern  void  hk_wow64_prepare_for_exception(PEXCEPTION_RECORD exception_record, PCONTEXT context);
typedef void(__stdcall* wow64_prepare_for_exception_fn)(PEXCEPTION_RECORD exception_record, PCONTEXT context);
 inline wow64_prepare_for_exception_fn o_wow64_prepare_for_exception = nullptr;
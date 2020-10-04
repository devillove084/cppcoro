///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_DETAIL_WIN32_HPP_INCLUDED
#define CPPCORO_DETAIL_WIN32_HPP_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCORO_CONFIG_HPP_INCLUDED
#define CPPCORO_CONFIG_HPP_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// Compiler Detection

#if defined(_MSC_VER)
# define CPPCORO_COMPILER_MSVC _MSC_FULL_VER
#else
# define CPPCORO_COMPILER_MSVC 0
#endif

#if defined(__clang__)
# define CPPCORO_COMPILER_CLANG (__clang_major__ * 10000 + \
                                 __clang_minor__ * 100 + \
                                 __clang_patchlevel__)
#else
# define CPPCORO_COMPILER_CLANG 0
#endif

#if defined(__GNUC__)
# define CPPCORO_COMPILER_GCC (__GNUC__ * 10000 + \
                               __GNUC_MINOR__ * 100 + \
                               __GNUC_PATCHLEVEL__)
#else
# define CPPCORO_COMPILER_GCC 0
#endif

/// \def CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
/// Defined to 1 if the compiler supports returning a coroutine_handle from
/// the await_suspend() method as a way of transferring execution
/// to another coroutine with a guaranteed tail-call.
#if CPPCORO_COMPILER_CLANG
# if __clang_major__ >= 7
#  define CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 1
# endif
#endif
#ifndef CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
# define CPPCORO_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 0
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_ASSUME(X) __assume(X)
#else
# define CPPCORO_ASSUME(X)
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_NOINLINE __declspec(noinline)
#elif CPPCORO_COMPILER_CLANG || CPPCORO_COMPILER_GCC
# define CPPCORO_NOINLINE __attribute__((noinline))
#else
# define CPPCORO_NOINLINE
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_FORCE_INLINE __forceinline
#elif CPPCORO_COMPILER_CLANG
# define CPPCORO_FORCE_INLINE __attribute__((always_inline))
#else
# define CPPCORO_FORCE_INLINE inline
#endif

/////////////////////////////////////////////////////////////////////////////
// OS Detection

/// \def CPPCORO_OS_WINNT
/// Defined to non-zero if the target platform is a WindowsNT variant.
/// 0x0500 - Windows 2000
/// 0x0501 - Windows XP/Server 2003
/// 0x0502 - Windows XP SP2/Server 2003 SP1
/// 0x0600 - Windows Vista/Server 2008
/// 0x0601 - Windows 7
/// 0x0602 - Windows 8
/// 0x0603 - Windows 8.1
/// 0x0A00 - Windows 10
#if defined(_WIN32_WINNT) || defined(_WIN32)
# if !defined(_WIN32_WINNT)
// Default to targeting Windows 10 if not defined.
#  define _WIN32_WINNT 0x0A00
# endif
# define CPPCORO_OS_WINNT _WIN32_WINNT
#else
# define CPPCORO_OS_WINNT 0
#endif

#if defined(__linux__)
# define CPPCORO_OS_LINUX 1
#else
# define CPPCORO_OS_LINUX 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CPU Detection

/// \def CPPCORO_CPU_X86
/// Defined to 1 if target CPU is of x86 family.
#if CPPCORO_COMPILER_MSVC
# if defined(_M_IX86)
#  define CPPCORO_CPU_X86 1
# endif
#elif CPPCORO_COMPILER_GCC || CPPCORO_COMPILER_CLANG
# if defined(__i386__)
#  define CPPCORO_CPU_X86 1
# endif
#endif
#if !defined(CPPCORO_CPU_X86)
# define CPPCORO_CPU_X86 0
#endif

/// \def CPPCORO_CPU_X64
/// Defined to 1 if the target CPU is x64 family.
#if CPPCORO_COMPILER_MSVC
# if defined(_M_X64)
#  define CPPCORO_CPU_X64 1
# endif
#elif CPPCORO_COMPILER_GCC || CPPCORO_COMPILER_CLANG
# if defined(__x86_64__)
#  define CPPCORO_CPU_X64 1
# endif
#endif
#if !defined(CPPCORO_CPU_X64)
# define CPPCORO_CPU_X64 0
#endif

/// \def CPPCORO_CPU_32BIT
/// Defined if compiling for a 32-bit CPU architecture.
#if CPPCORO_CPU_X86
# define CPPCORO_CPU_32BIT 1
#else
# define CPPCORO_CPU_32BIT 0
#endif

/// \def CPPCORO_CPU_64BIT
/// Defined if compiling for a 64-bit CPU architecture.
#if CPPCORO_CPU_X64
# define CPPCORO_CPU_64BIT 1
#else
# define CPPCORO_CPU_64BIT 0
#endif

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_CPU_CACHE_LINE std::hardware_destructive_interference_size
#else
// On most architectures we can assume a 64-byte cache line.
# define CPPCORO_CPU_CACHE_LINE 64
#endif

#endif

#if !CPPCORO_OS_WINNT
# error <cppcoro/detail/win32.hpp> is only supported on the Windows platform.
#endif

#include <utility>
#include <cstdint>

struct _OVERLAPPED;

namespace cppcoro
{
	namespace detail
	{
		namespace win32
		{
			using handle_t = void*;
			using ulongptr_t = std::uintptr_t;
			using longptr_t = std::intptr_t;
			using dword_t = unsigned long;
			using socket_t = std::uintptr_t;
			using ulong_t = unsigned long;

#if CPPCORO_COMPILER_MSVC
# pragma warning(push)
# pragma warning(disable : 4201) // Non-standard anonymous struct/union
#endif

			/// Structure needs to correspond exactly to the builtin
			/// _OVERLAPPED structure from Windows.h.
			struct overlapped
			{
				ulongptr_t Internal;
				ulongptr_t InternalHigh;
				union
				{
					struct
					{
						dword_t Offset;
						dword_t OffsetHigh;
					};
					void* Pointer;
				};
				handle_t hEvent;
			};

#if CPPCORO_COMPILER_MSVC
# pragma warning(pop)
#endif

			struct wsabuf
			{
				constexpr wsabuf() noexcept
					: len(0)
					, buf(nullptr)
				{}

				constexpr wsabuf(void* ptr, std::size_t size)
					: len(size <= ulong_t(-1) ? ulong_t(size) : ulong_t(-1))
					, buf(static_cast<char*>(ptr))
				{}

				ulong_t len;
				char* buf;
			};

			struct io_state : win32::overlapped
			{
				using callback_type = void(
					io_state* state,
					win32::dword_t errorCode,
					win32::dword_t numberOfBytesTransferred,
					win32::ulongptr_t completionKey);

				io_state(callback_type* callback = nullptr) noexcept
					: io_state(std::uint64_t(0), callback)
				{}

				io_state(void* pointer, callback_type* callback) noexcept
					: m_callback(callback)
				{
					this->Internal = 0;
					this->InternalHigh = 0;
					this->Pointer = pointer;
					this->hEvent = nullptr;
				}

				io_state(std::uint64_t offset, callback_type* callback) noexcept
					: m_callback(callback)
				{
					this->Internal = 0;
					this->InternalHigh = 0;
					this->Offset = static_cast<dword_t>(offset);
					this->OffsetHigh = static_cast<dword_t>(offset >> 32);
					this->hEvent = nullptr;
				}

				callback_type* m_callback;
			};

			class safe_handle
			{
			public:

				safe_handle()
					: m_handle(nullptr)
				{}

				explicit safe_handle(handle_t handle)
					: m_handle(handle)
				{}

				safe_handle(const safe_handle& other) = delete;

				safe_handle(safe_handle&& other) noexcept
					: m_handle(other.m_handle)
				{
					other.m_handle = nullptr;
				}

				~safe_handle()
				{
					close();
				}

				safe_handle& operator=(safe_handle handle) noexcept
				{
					swap(handle);
					return *this;
				}

				constexpr handle_t handle() const { return m_handle; }

				/// Calls CloseHandle() and sets the handle to NULL.
				void close() noexcept;

				void swap(safe_handle& other) noexcept
				{
					std::swap(m_handle, other.m_handle);
				}

				bool operator==(const safe_handle& other) const
				{
					return m_handle == other.m_handle;
				}

				bool operator!=(const safe_handle& other) const
				{
					return m_handle != other.m_handle;
				}

				bool operator==(handle_t handle) const
				{
					return m_handle == handle;
				}

				bool operator!=(handle_t handle) const
				{
					return m_handle != handle;
				}

			private:

				handle_t m_handle;

			};
		}
	}
}

#endif

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

void cppcoro::detail::win32::safe_handle::close() noexcept
{
	if (m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_handle);
		m_handle = nullptr;
	}
}

/* vim: set shiftwidth=2 tabstop=8 autoindent cindent expandtab: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is SSE.h
 *
 * The Initial Developer of the Original Code is the Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   L. David Baron <dbaron@dbaron.org>, Mozilla Corporation (original author)
 *   Justin Lebar <justin.lebar@gmail.com>, Mozilla Corporation
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* compile-time and runtime tests for whether to use SSE instructions */

#ifndef mozilla_SSE_h_
#define mozilla_SSE_h_

// for definition of NS_COM_GLUE
#include "nscore.h"

/**
 * The public interface of this header consists of a set of macros and
 * functions for Intel CPU features.
 *
 * DETECTING ISA EXTENSIONS
 * ========================
 *
 * This header provides the following functions for determining whether the
 * current CPU supports a particular instruction set extension:
 *
 *    mozilla::supports_mmx
 *    mozilla::supports_sse
 *    mozilla::supports_sse2
 *    mozilla::supports_sse3
 *    mozilla::supports_ssse3
 *    mozilla::supports_sse4a
 *    mozilla::supports_sse4_1
 *    mozilla::supports_sse4_2
 *
 * If you're writing code using inline assembly, you should guard it with a
 * call to one of these functions.  For instance:
 *
 *   if (mozilla::supports_sse2()) {
 *     asm(" ... ");
 *   }
 *   else {
 *     ...
 *   }
 *
 * Note that these functions depend on cpuid intrinsics only available in gcc
 * 4.3 or later and MSVC 8.0 (Visual C++ 2005) or later, so they return false
 * in older compilers.  (This could be fixed by replacing the code with inline
 * assembly.)
 *
 *
 * USING INTRINSICS
 * ================
 *
 * This header also provides support for coding using CPU intrinsics.
 *
 * For each mozilla::supports_abc function, we define a MOZILLA_MAY_SUPPORT_ABC
 * macro which indicates that the target/compiler combination we're using is
 * compatible with the ABC extension.  For instance, x86_64 with MSVC 2003 is
 * compatible with SSE2 but not SSE3, since although there exist x86_64 CPUs
 * with SSE3 support, MSVC 2003 only supports through SSE2.
 *
 * Until gcc fixes #pragma target [1] [2] or our x86 builds require SSE2,
 * you'll need to separate code using intrinsics into a file separate from your
 * regular code.  Here's the recommended pattern:
 *
 *  #ifdef MOZILLA_MAY_SUPPORT_ABC
 *    namespace mozilla {
 *      namespace ABC {
 *        void foo();
 *      }
 *    }
 *  #endif
 *
 *  void foo() {
 *    #ifdef MOZILLA_MAY_SUPPORT_ABC
 *      if (mozilla::supports_abc()) {
 *        mozilla::ABC::foo(); // in a separate file
 *        return;
 *      }
 *    #endif
 *
 *    foo_unvectorized();
 *  }
 *
 * You'll need to define mozilla::ABC::foo() in a separate file and add the
 * -mabc flag when using gcc.
 *
 * [1] http://gcc.gnu.org/bugzilla/show_bug.cgi?id=39787 and
 * [2] http://gcc.gnu.org/bugzilla/show_bug.cgi?id=41201 being fixed.
 *
 */

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

#ifdef __MMX__
  // It's ok to use MMX instructions based on the -march option (or
  // the default for x86_64 or for Intel Mac).
  #define MOZILLA_PRESUME_MMX 1
#endif
#ifdef __SSE__
  // It's ok to use SSE instructions based on the -march option (or
  // the default for x86_64 or for Intel Mac).
  #define MOZILLA_PRESUME_SSE 1
#endif
#ifdef __SSE2__
  // It's ok to use SSE2 instructions based on the -march option (or
  // the default for x86_64 or for Intel Mac).
  #define MOZILLA_PRESUME_SSE2 1
#endif
#ifdef __SSE3__
  // It's ok to use SSE3 instructions based on the -march option (or the
  // default for Intel Mac).
  #define MOZILLA_PRESUME_SSE3 1
#endif
#ifdef __SSSE3__
  // It's ok to use SSSE3 instructions based on the -march option.
  #define MOZILLA_PRESUME_SSSE3 1
#endif
#ifdef __SSE4A__
  // It's ok to use SSE4A instructions based on the -march option.
  #define MOZILLA_PRESUME_SSE4A 1
#endif
#ifdef __SSE4_1__
  // It's ok to use SSE4.1 instructions based on the -march option.
  #define MOZILLA_PRESUME_SSE4_1 1
#endif
#ifdef __SSE4_2__
  // It's ok to use SSE4.2 instructions based on the -march option.
  #define MOZILLA_PRESUME_SSE4_2 1
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
  #define MOZILLA_SSE_HAVE_CPUID_DETECTION
#endif

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64))

#if _MSC_VER >= 1400
  // MSVC 2005 or newer on x86 or amd64
  #define MOZILLA_SSE_HAVE_CPUID_DETECTION
#endif

#if defined(_M_IX86_FP)

#if _M_IX86_FP >= 1
  // It's ok to use SSE instructions based on the /arch option
  #define MOZILLA_PRESUME_SSE
#endif
#if _M_IX86_FP >= 2
  // It's ok to use SSE2 instructions based on the /arch option
  #define MOZILLA_PRESUME_SSE2
#endif

#elif defined(_M_AMD64)
  // MSVC for AMD64 doesn't support MMX, so don't presume it here.

  // SSE is always available on AMD64.
  #define MOZILLA_PRESUME_SSE
  // SSE2 is always available on AMD64.
  #define MOZILLA_PRESUME_SSE2
#endif

#elif defined(__SUNPRO_CC) && (defined(__i386) || defined(__x86_64__))
// Sun Studio on x86 or amd64

#define MOZILLA_SSE_HAVE_CPUID_DETECTION

#if defined(__x86_64__)
  // MMX is always available on AMD64.
  #define MOZILLA_PRESUME_MMX
  // SSE is always available on AMD64.
  #define MOZILLA_PRESUME_SSE
  // SSE2 is always available on AMD64.
  #define MOZILLA_PRESUME_SSE2
#endif

#endif

namespace mozilla {

  namespace sse_private {
#if defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#if !defined(MOZILLA_PRESUME_MMX)
    extern bool NS_COM_GLUE mmx_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE)
    extern bool NS_COM_GLUE sse_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE2)
    extern bool NS_COM_GLUE sse2_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE3)
    extern bool NS_COM_GLUE sse3_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSSE3)
    extern bool NS_COM_GLUE ssse3_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE4A)
    extern bool NS_COM_GLUE sse4a_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE4_1)
    extern bool NS_COM_GLUE sse4_1_enabled;
#endif
#if !defined(MOZILLA_PRESUME_SSE4_2)
    extern bool NS_COM_GLUE sse4_2_enabled;
#endif
#endif
  }

#if defined(MOZILLA_PRESUME_MMX)
#define MOZILLA_MAY_SUPPORT_MMX 1
  inline bool supports_mmx() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#if !(defined(_MSC_VER) && defined(_M_AMD64))
  // Define MOZILLA_MAY_SUPPORT_MMX only if we're not on MSVC for
  // AMD64, since that compiler doesn't support MMX.
#define MOZILLA_MAY_SUPPORT_MMX 1
#endif
  inline bool supports_mmx() { return sse_private::mmx_enabled; }
#else
  inline bool supports_mmx() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE)
#define MOZILLA_MAY_SUPPORT_SSE 1
  inline bool supports_sse() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE 1
  inline bool supports_sse() { return sse_private::sse_enabled; }
#else
  inline bool supports_sse() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE2)
#define MOZILLA_MAY_SUPPORT_SSE2 1
  inline bool supports_sse2() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE2 1
  inline bool supports_sse2() { return sse_private::sse2_enabled; }
#else
  inline bool supports_sse2() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE3)
#define MOZILLA_MAY_SUPPORT_SSE3 1
  inline bool supports_sse3() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE3 1
  inline bool supports_sse3() { return sse_private::sse3_enabled; }
#else
  inline bool supports_sse3() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSSE3)
#define MOZILLA_MAY_SUPPORT_SSSE3 1
  inline bool supports_ssse3() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSSE3 1
  inline bool supports_ssse3() { return sse_private::ssse3_enabled; }
#else
  inline bool supports_ssse3() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE4A)
#define MOZILLA_MAY_SUPPORT_SSE4A 1
  inline bool supports_sse4a() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE4A 1
  inline bool supports_sse4a() { return sse_private::sse4a_enabled; }
#else
  inline bool supports_sse4a() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE4_1)
#define MOZILLA_MAY_SUPPORT_SSE4_1 1
  inline bool supports_sse4_1() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE4_1 1
  inline bool supports_sse4_1() { return sse_private::sse4_1_enabled; }
#else
  inline bool supports_sse4_1() { return false; }
#endif

#if defined(MOZILLA_PRESUME_SSE4_2)
#define MOZILLA_MAY_SUPPORT_SSE4_2 1
  inline bool supports_sse4_2() { return true; }
#elif defined(MOZILLA_SSE_HAVE_CPUID_DETECTION)
#define MOZILLA_MAY_SUPPORT_SSE4_2 1
  inline bool supports_sse4_2() { return sse_private::sse4_2_enabled; }
#else
  inline bool supports_sse4_2() { return false; }
#endif

}

#endif /* !defined(mozilla_SSE_h_) */
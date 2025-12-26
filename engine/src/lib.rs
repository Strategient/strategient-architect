//! Strategient-Architect Engine
//!
//! Rust engine crate providing core functionality via C ABI.
//! Pattern: "Rust allocates, Rust frees" for safe string transfer.

use std::ffi::{c_char, CString};

/// Engine version string - returned to C++ caller.
const ENGINE_VERSION: &str = "0.1.0";

/// Returns the engine version as a C string.
/// Caller MUST call `engine_free_string` to release memory.
#[no_mangle]
pub extern "C" fn engine_version() -> *mut c_char {
    let version = CString::new(ENGINE_VERSION).expect("CString::new failed");
    version.into_raw()
}

/// Frees a string previously allocated by the engine.
/// # Safety
/// - ptr must be a valid pointer returned by engine functions (e.g., engine_version)
/// - ptr must not be null
/// - ptr must not have been freed already
#[no_mangle]
pub unsafe extern "C" fn engine_free_string(ptr: *mut c_char) {
    if !ptr.is_null() {
        // Safety: ptr was allocated via CString::into_raw
        drop(CString::from_raw(ptr));
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CStr;

    #[test]
    fn test_engine_version() {
        let ptr = engine_version();
        assert!(!ptr.is_null());
        
        let c_str = unsafe { CStr::from_ptr(ptr) };
        let version = c_str.to_str().expect("Invalid UTF-8");
        assert_eq!(version, ENGINE_VERSION);
        
        unsafe { engine_free_string(ptr) };
    }
}


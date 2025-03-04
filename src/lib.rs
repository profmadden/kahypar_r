use std::os::raw::{c_int, c_uint, c_ulong, c_float};

use cty::c_double;

#[cxx::bridge(namespace = "kahypar")]
mod ffi {
    unsafe extern "C++" {}
}

extern "C" {
    pub fn kahypar_hello();
    pub fn partition(nvtxs: c_uint, nhedges: c_uint, hewt: *const c_int, vtw: *const c_int, eind: *const c_ulong, eptr: *const c_uint, part: *mut c_int, kway: c_int, passes: c_int, seed: c_ulong, imbalance: c_float);
    pub fn mtpartition(nvtxs: c_uint, nhedges: c_uint, hewt: *const c_int, vtw: *const c_int, eind: *const c_ulong, eptr: *const c_uint, part: *mut c_int, kway: c_int, passes: c_int, seed: c_ulong, imbalance: c_float);    
}


pub fn kahypar_test(data: Vec<i32>) {
    println!("Here we are with KaHyPar!");
    for v in &data {
        println!("Data value {}", v);
    }
    unsafe {
	kahypar_hello();
    }
}

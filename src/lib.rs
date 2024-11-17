use std::os::raw::{c_int, c_uint, c_ulong};

use cty::c_double;

#[cxx::bridge(namespace = "kahypar")]
mod ffi {
    unsafe extern "C++" {}
}

extern "C" {
    pub fn kahypar_hello();
    pub fn partition(nvtxs: c_uint, nhedges: c_uint, hewt: *const c_int, vtw: *const c_int, eind: *const c_ulong, eptr: *const c_uint, part: *mut c_int, kway: c_int);
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


fn main() {
    println!("Hello, world!");
    unsafe {
	kahypar_hello();
    }
}

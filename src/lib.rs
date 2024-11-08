use std::os::raw::{c_int, c_uint};

use cty::c_double;

#[cxx::bridge(namespace = "kahypar")]
mod ffi {
    unsafe extern "C++" {}
}

extern "C" {
    pub fn kahypar_hello();
    pub fn kahypar_array(data: *const c_int, length: c_uint);
    pub fn kahypar_partition(num_vertices: c_int, num_hyperedges: c_uint
    );
}

pub fn kahypar_test(data: Vec<i32>) {
    println!("Here we are with KaHyPar!");
    for v in &data {
        println!("Data value {}", v);
    }
    unsafe {
	kahypar_hello();
    kahypar_array(data.as_ptr() as *const c_int, data.len() as c_uint);

    }
}

pub fn kahypar_part(h_wt: Vec<c_uint>, h_idx:Vec<c_uint>, he:Vec<c_uint>) {
    unsafe {
        // kahypar_partition();
    }
}

fn main() {
    println!("Hello, world!");
    unsafe {
	kahypar_hello();
    }
}

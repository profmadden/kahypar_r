

#[cxx::bridge(namespace = "kahypar")]
mod ffi {
    unsafe extern "C++" {}
}

extern "C" {
    pub fn kahypar_hello();
    
/*
    pub fn flute_init(); // Loads in the LUT                                                                     
    pub fn flute_new_tree(); // Preps for a new tree                                                             
    pub fn flute_scale(xscale: cty::c_int, yscale: cty::c_int);
    pub fn flute_add_xy(x: cty::c_int, y: cty::c_int);
    pub fn flute_build_tree() -> cty::c_int;
    pub fn flute_get_e() -> cty::c_int;
    pub fn flute_get_ex1(e: cty::c_int) -> cty::c_int;
    pub fn flute_get_ey1(e: cty::c_int) -> cty::c_int;
    pub fn flute_get_ex2(e: cty::c_int) -> cty::c_int;
    pub fn flute_get_ey2(e: cty::c_int) -> cty::c_int;
*/
}

pub fn kahypar_test() {
    println!("Here we are with KaHyPar!");
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

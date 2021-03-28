extern crate libc;

use libc::c_int;
use libc::size_t;
use libc::c_void;
use libc::c_char;
// use std::ffi::CString;

pub enum PMEMobjpool {}
pub static mut my_pop: Option<*mut PMEMobjpool>= None;


#[link(name = "cnt_store")]
extern "C"{
    fn create_content(pop: *mut PMEMobjpool, size: size_t, buf: *mut c_char, id: c_int)->c_int;
    fn print_content(pop: *mut PMEMobjpool, id: c_int)->c_void;
    fn print_content_all(pop: *mut PMEMobjpool)->c_void;
    fn my_free_fn(pop: *mut PMEMobjpool, id: c_int)->c_void;
    fn my_free_all_fn(pop: *mut PMEMobjpool)->c_void;
    fn init(path:*const c_char)->*mut PMEMobjpool;
    fn fin(pop:*mut PMEMobjpool)->c_void;
    fn write_at_content(pop: *mut PMEMobjpool, id: c_int, offset: c_int, buf: *mut c_char, size: size_t)->c_int;
    fn write_content(pop: *mut PMEMobjpool, id: c_int, buf: *mut c_char, size: size_t)->c_int;
    fn read_at_content(pop: *mut PMEMobjpool, id: c_int, offset: c_int, buf: *mut c_char)->c_int;
    fn read_content(pop: *mut PMEMobjpool, id: c_int, buf: *mut c_char)->c_int;
}

pub fn my_create_content(pop: *mut PMEMobjpool, size: usize, buf: *mut c_char, id: u32)->c_int{
    unsafe{
        create_content(pop, size, buf, id as c_int)
    }
}

pub fn my_print_content(pop: *mut PMEMobjpool, id: u32)->c_void{
    unsafe{
        print_content(pop, id as c_int)
    }
}

pub fn my_print_content_all(pop: *mut PMEMobjpool)->c_void{
    unsafe{
        print_content_all(pop)
    }
}

pub fn my_free(pop: *mut PMEMobjpool, id: u32)->c_void{
    unsafe{
        my_free_fn(pop, id as c_int)
    }
}

pub fn my_free_all(pop: *mut PMEMobjpool)->c_void{
    unsafe{
        my_free_all_fn(pop)
    }
}

pub fn my_init(path:*const u8)->*mut PMEMobjpool{
    unsafe{
        init(path as *const c_char)
    }
}

pub fn my_fin(pop:*mut PMEMobjpool)->c_void{
    unsafe{
        fin(pop)
    }
}

pub fn my_write_at_content(pop: *mut PMEMobjpool, id: u32, offset: usize, buf: *const u8, size: usize)->c_int{
    unsafe{
        write_at_content(pop, id as c_int, offset as c_int, buf as *mut c_char, size as size_t)
    }
}

pub fn my_write_content(pop: *mut PMEMobjpool, id: u32, buf: *mut c_char, size: usize)->c_int{
    unsafe{
        write_content(pop, id as c_int, buf as *mut c_char, size as size_t)
    }
}

pub fn my_read_at_content(pop: *mut PMEMobjpool, id: u32, offset: usize, buf: *const u8)->c_int{
    unsafe{
        read_at_content(pop, id as c_int, offset as c_int, buf as *mut c_char)
    }
}

pub fn my_read_content(pop: *mut PMEMobjpool, id: u32, buf: *mut c_char)->c_int{
    unsafe{
        read_content(pop, id as c_int, buf as *mut c_char)
    }
}


mod file;
// use std::sync::Arc;
use std::env;
// use fuse::{FileType, FileAttr, Filesystem, Request, ReplyData, ReplyEntry, ReplyAttr, ReplyDirectory};
// use std::ffi::OsStr;
use crate::file::MyFuseFs;
use crate::file::RamFs;

mod ffi_wrapper;

// use crate::ffi_wrapper::PMEMobjpool;
use crate::ffi_wrapper::my_pop;
fn main() {
    // env_logger::init();
    // let mountpoint = env::args_os().nth(1).unwrap();
    // let pool_addr = env::args_os().nth(2).unwrap();
    // my_pop = ffi_wrapper::my_init(pool_addr);
    let args: Vec<String> = env::args().collect();
    let mountpoint = &args[1];
    let pool_addr = &args[2];
    unsafe{
        my_pop =match ffi_wrapper::my_init(pool_addr.as_ptr()){
            val => Some(val),
        // _ => None,
        }
    };
    
    // let options = ["-o", "ro", "-o", "fsname=hello"]
    //     .iter()
    //     .map(|o| o.as_ref())
    //     .collect::<Vec<&OsStr>>();
    // fuse::mount(*(MyFuseFs::new()).clone(), &mountpoint, &options).expect("failed to mount");
    let fs = RamFs::new();
    fuse::mount(
        MyFuseFs::new(fs),
        &mountpoint,
        &[]
    ).expect("fail to mount");
}
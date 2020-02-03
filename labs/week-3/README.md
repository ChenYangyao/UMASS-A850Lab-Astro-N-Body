# Lab: IO with HDF5

[Back to Home](https://github.com/ChenYangyao/N-Body-Course) | [View Source Code](./src) | [View pdf images](./out)

## Introduction

HDF5 is a file format, a data organization/access model, and also a suite of software.

- The file format of HDF5 is stanadarized by `HDF5 File Format Specification`. It specifies the bit-level storage layout of data, and hides many details that is unnecessarily exposed to user.

- The `HDF5 Abstract Data Model` specify the building blocks of HDF5 file - the HDF5 object. Two primary objects in HDF5 is group and dataset.

- The HDF5 community provide a complete set of softwares and language interfaces. C, C++, Fortran, Python and Java users can all use HDF5 to manipulate the same data file.

The download and documentation can be found at HDF group [home page](https://portal.hdfgroup.org/display/support/Documentation?utm_source=hdfhomepage).

## Lab - Timing the IO with Different Tools
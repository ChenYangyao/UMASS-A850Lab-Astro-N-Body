# Lab: IO with HDF5

[Back to home](https://github.com/ChenYangyao/N-Body-Course) | [View Jupyter](./out/perf_measure.ipynb) | [View pdf images](./out) | [View source code](./src)

## Introduction

HDF5 is a file format, a data organization/access model, and also a suite of software.

- The file format of HDF5 is stanadarized by `HDF5 File Format Specification`. It specifies the bit-level storage layout of data, and hides many details that is unnecessarily exposed to user.

- The `HDF5 Abstract Data Model` specify the building blocks of HDF5 file - the HDF5 object. Two primary objects in HDF5 is group and dataset.

- The HDF5 community provide a complete set of softwares and language interfaces. C, C++, Fortran, Python and Java users can all use HDF5 to manipulate the same data file.

The download and documentation can be found at HDF group [home page](https://portal.hdfgroup.org/display/support/Documentation?utm_source=hdfhomepage).

## Lab - Timing the IO with Different Tools

<table><tr>
    <td><img src="./out/tune_chunk.png" style="border: none;"></td>
    <td style="text-align:top;font-size: small;" width="300">
        <em>the IO performance with HDF5 with chunk data-layout. </em><br> 
        We perform IO of many "Chunk_IO"-size chunks, 
        on a 128 MiB dataset with "Chunk_Storage=8 KiB" storage chunks. 
        The default data access parameter is listed in the lower-right panel.
        In each panel, one data access parameter is adjusted and the IO thoughtput
        is measured with different parameter values. All these test is done on 
        the local DELL PC with Linux Kernel Version 4.15.
    </td>
</tr></table>

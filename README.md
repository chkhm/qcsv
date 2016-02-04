   QCSV v.1: Quick CSV File Analysis
===============================================
 
qcsv is a tool for manipulating comma separated values (CSV) files or streams. It is able to detect
columns, column types and the separator between the columns of the CSV file. It provide features to
manipulate the file, e.g. to strip or add white spaces, strip or add quotes, reformat columns (e.g.
change date and time representation), but most importantly to manipulate the data itself: Similar 
to the SQL select command individual columns can be chosen (select a, b, c) and a where clause can 
be applied that filters out individual rows. In addition qcsv can smooth the incoming data which 
can be handy when you want to plot it. Smoothing means that the data is averaged over multiple 
rows. qcsv can smooth integer, floating point and datetime data. For strings one string out of the 
group of rows is chosen.

qcsv operates stream-oriented and can work with input files of arbitrary length. The user can 
manipulate how much data shall be kept in RAM. When no input and/ or output file is specified it 
reads from standard input and writes to standard output.

In the following you find an example way to call it from the command line:
 
qcsv --select "1,2,3,6:15" --smooth 2 --filter "#2 = 'Abce' and #3 >= 10" -r"#1:'2015-30-01'" -O"by_avg [6:15]" infile.csv outfile.csv

This will analyze the file "infile.csv" and write the results of the analysis into the file "outfile.csv".

It will automatically detect the column separator and it will automatically find out if strings are
quoted and which character is used to quote them.

The analysis will cover the columns 1,2, and 3 as well as the column 6 to 15 of the infile. Of those columns only rows are selected
in which the value of the second column is "Abce" and the value of the third column is larger or equal to 10.
The values will be smoothed over two rows, which means the average of every 2 rows is being used in the output 
file.

Before output the first column is rebased to the date 2015-30-01: This means that the first rows value is 
replaced with the date 2015-30-01. In all subsequent rows the value is modified according to their distance to
the first row.

No limitation is given regarding the statistics, which means the output file will include the complete 
statistics set including:
- Total
- Mean
- standard deviation
- Maximum
- Index of maximum value
- Minimum
- Index of minimum value
- Mean distance between rows for numeric and date/time rows
- Standard Deviation of distance between rows
- Most frequent string (for string columns)

Also the smoothed data will be printed with the default column separator (i.e. a comma ","), with spaces to
improve human readability, and strings will be put into quotes. 

The columns are sorted before output: Rows 1,2,3 come first in unmodified order. However, the rows 6-15 are ordered 
according to their average value. The column with the lowest average value comes first, the column with the 
largest value comes last.

Please refer to the man page for further details of the command line parameters of QCSV.



Installation
============

To install the precompiled executables available for download the installers from this web site.
We have an installer available for Ubuntu Linux, Mac OS, and Windows. For Linux and windows there is
a 32 bit and 64 bit version available for download. For MacOS we only offer a 64 bit version.

If you want to compile the source code yourself please review the operating system specific installation 
documents.The software is mainly developed under Linux and Mac OS. We do provide a makefile that compiles 
QCSV under Ubuntu Linux with a simple: 
make

For MacOS we provide an Xcode project file. For compiling under Windows we offer a Visual Studio Project File.
The Visual Studio File is not tested as often as the XCode project file and the makefile, so it might be slightly
outdated. Please note that you also need an extra file from other open source projects (see below).


Support
=======

If you need help with QCSV or want to ask a question about QCSV, use the the mailing list. Please also check
for answers to your problems on the FAQ page of QCSV. This is a non-commercial project so please be patient. We 
will do our best to answer questions.  


Contributing to QCSV
====================

Please contact us via the github QCSV mailing list if you would like to contribute to this project. We welcome 
developers with time to improve and extend QCSV.


Copying and License
===================

This material is copyright (c) 2015 Siemens Corporation, Corporate Technology.
It is released and licensed under a dual license model. Designed to meet the requirements of different users, 
QSCV is available under two different license models: The GNU Affero General Public License (AGPL) v.3.0 or a commercial license.

The GNU Affero General Public License (AGPL) v3.0 whose full text 
may be found at:

http://www.fsf.org/licensing/licenses/agpl-3.0.html

If licensed under the AGPL v.3.0, this program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. 
If not, see <http://www.gnu.org/licenses/>.

If you are interested in acquiring a commercial license, please email CTUSLegal@Siemens.com.

Note
====

QCSV is written in C++ and relies heavily on features of C++-14. A reasonably new version of GCC or Clang will 
do just fine.

We do not rely on other software packages with one exception:
Microsoft Visual Studio does not provide an implementation for the Posix API strptime.h. You therefore need to 
download an implementation of strptime from one of the many open source projects. We recommend using the 
implementation from this project:


Oen Source Implementation of Posix API: strptime.h:  https://github.com/tnodir/luasys/blob/master/src/win32/strptime.c
(NetBSD License)

Simply download the file and add it to your compile path. We cannot include the file in our project directly.


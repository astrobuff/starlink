/*
 *    This file is part of dvi2bitmap.
 *    Copyright 1999--2002, Council for the Central Laboratory of the Research Councils
 *    
 *    This program is part of the Starlink Software Distribution: see
 *    http://www.starlink.ac.uk 
 *
 *    dvi2bitmap is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dvi2bitmap is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dvi2bitmap; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    The General Public License is distributed along with this
 *    program in the file LICENCE.
 *
 *    Author: Norman Gray <norman@astro.gla.ac.uk>
 *    $Id$
 */


#ifndef INPUT_BYTE_STREAM_HEADER_READ
#define INPUT_BYTE_STREAM_HEADER_READ 1

#include <Byte.h>
#include <DviError.h>
#include <verbosity.h>

#include <string>

class InputByteStreamError : public DviError {
 public:
    InputByteStreamError(string s) : DviError(s) { }
};

/**
 * Abstracts a file as a stream of bytes.
 */
class InputByteStream {

 public:
    InputByteStream(int fileno)
	    throw (InputByteStreamError);
    InputByteStream(string srcspec)
	    throw (InputByteStreamError);
    ~InputByteStream();

    bool eof();
    Byte getByte(void)
	    throw (InputByteStreamError);
    const Byte *getBlock(unsigned int length)
	    throw (InputByteStreamError);
    void skip (unsigned int)
	    throw (InputByteStreamError);

    signed int getSIU(int)
	    throw (InputByteStreamError);
    signed int getSIS(int)
	    throw (InputByteStreamError);
    unsigned int getUIU(int)
	    throw (InputByteStreamError);
    /*
     * static getUIU reads from an array, rather than the file (ie,
     * it's really nothing to do with InputByteStream, but it's here
     * for consistency.
     */
    static unsigned int getUIU(int, const Byte *)
	    throw (InputByteStreamError);
    static void setDefaultBufferSize(unsigned int length);
    /**
     * Sets the verbosity of this module.
     * @param level the required verbosity
     */
    static inline void verbosity (const verbosities level) {
	verbosity_ = level;
    }

 protected:
    InputByteStream();
    bool bindToFileDescriptor(int fileno,
			      string filename="",
			      int bufsize=0,
			      bool fillBufferAndClose=false)
	    throw (InputByteStreamError);
    int openSourceSpec(string srcspec)
	    throw (InputByteStreamError);
    /**
     * Returns the file descriptor this stream is bound to.  If there
     * is no open descriptor (see the <code>fillBufferAndClose</code>
     * parameter to {@link #bindToFileDescriptor}), returns negative.
     *
     * @return the file descriptor, or negative if the descriptor has
     * been closed
     */
    int getFD(void) const { return fd_; }
    void bufferSeek(unsigned int pos)
	    throw (InputByteStreamError);
    void reloadBuffer(void);
    /**
     * Register a function to be called when the stream file
     * descriptor is closed.  Note that this is not the same as being
     * called when the stream reaches EOF, which might happen more
     * than once if an extending class has, for example, reset the
     * file pointer after EOF.
     *
     * @param fn callback function
     */
    void closedFD(void);
    /*    void closeCallback(void (*fn)(void));*/
/*     void closeCallback(void (*fn)(void)) { */
/* 	close_callback_ = fn; */
/* 	//cerr << "registered close_callback_=" << fn << endl; */
/*     } */
    /**
     * Returns the verbosity setting of this class
     */
    verbosities getVerbosity(void) const { return verbosity_; };

 private:
    int fd_;
    string fname_;
    size_t buflen_;
    Byte *buf_;			/* buffer */
    Byte *p_;			/* current position in buffer */
    Byte *eob_;			/* end of buffer */
    bool eof_;			/* true on end of file */
    void (*close_callback_)(void);
    void read_buf_(void)
	    throw (InputByteStreamError);
    size_t certainly_read_(int fd, Byte *b, size_t len)
	    throw (InputByteStreamError);
    void close_fd_(void);

    static verbosities verbosity_;
    static unsigned int default_buffer_length_;
};

#endif /* INPUT_BYTE_STREAM_HEADER_READ */

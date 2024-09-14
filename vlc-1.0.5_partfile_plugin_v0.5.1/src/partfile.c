/*****************************************************************************
 * file.c: file input (file: access plug-in)
 *****************************************************************************
 * Copyright (C) 2001-2006 the VideoLAN team
 * Copyright © 2006-2007 Rémi Denis-Courmont
 * $Id: ad111cb458c35af33a8accb6c172dc4ae7ffad13 $
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Rémi Denis-Courmont <rem # videolan # org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * eMule part file input plugin v0.5.1 for VideoLAN Client v1.0.5
 *****************************************************************************
 *   Versions >= 0.5.0 by http://www.emule-project.net
 *   Versions >= 0.2.0 by Cybermutant
 *   Previous versions by bluecow ( http://www.emule-project.net )
 *****************************************************************************/

/*****************************************************************************
 * History
 *****************************************************************************
 * The plugin was first created by bluecow for the 0.6.1 version of VideoLAN Client,
   based on VLC's file access plugin code (/vlc-0.6.1/modules/access/file.c)

 * The plugin was maintained and updated by bluecow for VLC versions 0.6.2, 0.7.0,
   0.7.1, and 0.7.2.

 * 0.3.x updated the version of the plugin so that it worked with VLC v0.8.2 and on,
   created by merging bluecow's code with the new VLC file access module.

 * 0.5.x is a port to VLC 1.0.5 with some changes to the handling (and with a lot room
    for further improvements left)

 * For detailed changelog see the CHANGELOG.htm file
 *****************************************************************************/
/*****************************************************************************
 * Preamble
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define MODULE_STRING	"emulepartfile"
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define __CONCAT(x,y) x ## y
#define INT64_C(value) __CONCAT(value, LL) 
#define _(str)  (str)
#define N_(str) (str)
#define I64Fd "%I64d"
#ifndef snprintf
#define snprintf _snprintf
#endif


#pragma warning(disable:4996)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_input.h>
#include <vlc_access.h>
#include <vlc_dialog.h>

#include <assert.h>
#include <errno.h>
#ifdef HAVE_SYS_TYPES_H
#   include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#   include <fcntl.h>
#endif
#if defined (__linux__)
#   include <sys/vfs.h>
#   include <linux/magic.h>
#elif defined (HAVE_SYS_MOUNT_H)
#   include <sys/param.h>
#   include <sys/mount.h>
#endif

#if defined( WIN32 )
#   include <io.h>
#   include <ctype.h>
#   include <shlwapi.h>
#else
#   include <unistd.h>
#   include <poll.h>
#endif

#if defined( WIN32 ) && !defined( UNDER_CE )
#   ifdef lseek
#      undef lseek
#   endif
#   define lseek _lseeki64
#elif defined( UNDER_CE )
/* FIXME the commandline on wince is a mess */
# define dup(a) -1
# define PathIsNetworkPathW(wpath) (! wcsncmp(wpath, L"\\\\", 2))
#endif

#include <vlc_charset.h>

#define DEBUG_MODE p_sys->b_verbose
#define DEBUG_MSG(...) if (DEBUG_MODE) { msg_Info( p_access, __VA_ARGS__ ); }
#define READ_GRACESKIP	102400 // how much to increase gaps to make sure really hitting data areas

/*****************************************************************************
 * Function for displaying information in "Stream and Media Info"
 * (copied from cdda/info.c and modified for the part file plugin)
 *
 * Adds a string-valued entry to the "Stream and Media Info"
 * under category "eMule part file details"
 * if the string is not null or the null string.
 *****************************************************************************/

#define add_p_met_info_str( P_INPUT, TITLE, FIELD ) \
    if ( FIELD && strlen( FIELD ) )                 \
    {                                               \
        input_Control( P_INPUT, INPUT_ADD_INFO,     \
                   "eMule part file details",       \
                   _(TITLE), "%s", FIELD );         \
    }

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
static int  Open ( vlc_object_t * );
static void Close( vlc_object_t * );

#define CACHING_TEXT N_("Caching value in ms")
#define CACHING_LONGTEXT N_( \
    "Caching value for files. This " \
    "value should be set in milliseconds." )

vlc_module_begin()
    set_description( _("eMule part file input") );
    set_shortname( _("eMule Part File") );
    set_category( CAT_INPUT );
    set_subcategory( SUBCAT_INPUT_ACCESS );
    add_usage_hint( N_("Use for previewing eMule part files") );

    /* ------------ */
    add_bool( "mpg_partfile_stream", true, NULL,
        N_("Enable plugin for MPEG part files"),
        N_("Specifies if this plugin shall provide a media stream with the information found in "
           "an eMule part.met file."),
        false );

    add_bool( "mpg_partfile_seekable", true, NULL,
        N_("Provide a seekable stream for MPEG part files"),
        N_("Specifies if the MPEG media stream built from the eMule part file will be seekable."),
        false );

    /* ------------ */
    add_bool( "avi_partfile_stream", true, NULL,
        N_("Enable plugin for AVI part files"),
        N_("Specifies if this plugin shall provide a media stream with the information found in "
           "an eMule part.met file."),
        false );

    add_bool( "avi_partfile_seekable", true, NULL,
        N_("Provide a seekable stream for AVI part files"),
        N_("Specifies if the AVI media stream built from the eMule part file will be seekable."),
        false );

    /* ------------ */
    add_bool( "oth_partfile_stream", false, NULL,
        N_("Enable plugin for other (non MPEG or AVI) part files"),
        N_("Specifies if this plugin shall provide a media stream with the information found in "
           "an eMule part.met file."),
        false );

    add_bool( "oth_partfile_seekable", false, NULL,
        N_("Provide a seekable stream for other (non MPEG or AVI) part files"),
        N_("Specifies if the media stream built from the eMule part file will be seekable."),
        false );

    /* ------------ */
    add_bool( "generate_debug_msg", false, NULL,
        N_("Generate debug messages"),
        N_("Writes verbose debugging information to the VLC Messages window."),
        true );

    add_integer( "file-caching", DEFAULT_PTS_DELAY / 1000, NULL, CACHING_TEXT, CACHING_LONGTEXT, true );
    set_capability( "access", 51 ); /* Set to a number >50 to be called before the 'access_file' module */
    add_shortcut( "partfile" );      /* Can be used for an access protocol e.g. "partfile://D:\Temp\001.part" */
    set_callbacks( Open, Close );
	add_obsolete_string( "file-cat" )
vlc_module_end()

/*****************************************************************************
 * Exported prototypes
 *****************************************************************************/
static int  Seek( access_t *, int64_t );
static int  NoSeek( access_t *, int64_t );
static ssize_t Read( access_t *, uint8_t *, size_t );
static int  Control( access_t *, int, va_list );

static int  open_file( access_t *, const char * );

typedef struct
{
    int64_t  l_start;
    int64_t  l_end;
} GAP;

typedef struct
{
    int64_t  l_start;
    int64_t  l_end;
} AVAIL;

enum EFileType
{
    ftNUL = 0,
    ftMPG,
    ftAVI
};

struct access_sys_t
{
	unsigned int i_nb_reads;

	int		   fd;
	int        fd_backup;
	/* */
	bool	   b_pace_control;


	bool	   b_verbose;

	int64_t    l_part_file_size;
	int64_t    l_avail_size;
	int64_t    l_real_pos;
	int		   l_currentgap;
	int		   l_jumps;
	mtime_t	   l_lastcorrectionjump;
		
	uint32_t   i_media_length;
	uint32_t   i_media_bitrate;

	int        i_gaps;
	GAP *      p_gaps;

	int        i_avails;
	AVAIL *    p_avails;

	enum EFileType  n_file_type;

	input_thread_t  *p_input; /* For accessing Stream and Media Info window */
};
// Constants from opcodes.h in eMule sources

#define PARTFILE_VERSION           0xe0
#define PARTFILE_VERSION_LARGEFILE 0xe2
// File tags
#define FT_FILENAME       0x01   // <string>
#define FT_FILESIZE       0x02   // <uint32> (or <uint64> for large files)
#define FT_FILETYPE       0x03   // <string>
#define FT_FILEFORMAT     0x04   // <string>
#define FT_GAPSTART       0x09   // <uint32> (or <uint64> for large files)
#define FT_GAPEND         0x0A   // <uint32> (or <uint64> for large files)
#define FT_MEDIA_ARTIST   0xD0   // <string>
#define FT_MEDIA_ALBUM    0xD1   // <string>
#define FT_MEDIA_TITLE    0xD2   // <string>
#define FT_MEDIA_LENGTH   0xD3   // <uint32>
#define FT_MEDIA_BITRATE  0xD4   // <uint32>
#define FT_MEDIA_CODEC    0xD5   // <string>
// Tag types
#define TAGTYPE_STRING    0x02
#define TAGTYPE_UINT32    0x03
#define TAGTYPE_FLOAT32   0x04
#define TAGTYPE_UINT64    0x0B

static bool IsRemote (int fd)
{
    return false;
}

static int CmpGaps( const void* p0, const void* p1 )
{
    if( ((const GAP*)p0)->l_start < ((const GAP*)p1)->l_start )
        return -1;
    if( ((const GAP*)p0)->l_start > ((const GAP*)p1)->l_start )
        return 1;
    return 0;
}

static void SecToTimeLength( uint32_t l_sec, char* buff, size_t size )
{
    if( l_sec >= 3600 )
    {
      unsigned uHours = l_sec / 3600;
      unsigned uMin = ( l_sec - uHours * 3600 ) / 60;
      unsigned uSec = l_sec - uHours * 3600 - uMin * 60;
      snprintf( buff, size, "%u:%02u:%02u", uHours, uMin, uSec );
    }
    else
    {
      unsigned uMin = l_sec / 60;
      unsigned uSec = l_sec - uMin * 60;
      snprintf( buff, size, "%u:%02u", uMin, uSec );
    }
}

static int read_part_met_file( access_t* p_access, access_sys_t* p_sys, int file )
{
    uint8_t  c_version;
    uint16_t n_part_hashes;
    uint32_t l_meta_tags;
    uint32_t l_tag;
    int64_t  l_gap_start;
    int64_t  l_total_missing;
    int64_t  l_file_pos;
    int64_t  l_last_end;
    int i;
    char sz_filename[_MAX_PATH];
    char sz_fileformat[_MAX_PATH];
    const char* psz_format = NULL;
    char* psz_media_artist = NULL;
    char* psz_media_album = NULL;
    char* psz_media_title = NULL;

    DEBUG_MSG("Reading part.met file");

    /* BYTE: Version */
    if( read( file, &c_version, sizeof( c_version ) ) != sizeof( c_version ) )
    {
        msg_Err( p_access, "Failed to read file version" );
        return 0;
    }
    if( c_version != PARTFILE_VERSION && c_version != PARTFILE_VERSION_LARGEFILE )
    {
        msg_Err( p_access, "Unsupported file version 0x%02x !!", c_version );
        return 0;
    }

    /* INT32: Date */
    if( lseek( file, 4, SEEK_CUR ) == -1 )
    {
        msg_Err( p_access, "Failed to read date" );
        return 0;
    }

    /* BYTE * 16: File ID */
    if( lseek( file, 16, SEEK_CUR ) == -1 )
    {
        msg_Err( p_access, "Failed to read file hash" );
        return 0;
    }

    /* UINT16: Number of part hashes */
    if( read( file, &n_part_hashes, sizeof( n_part_hashes ) ) != sizeof( n_part_hashes ) )
    {
        msg_Err( p_access, "Failed to read number of part hashes" );
        return 0;
    }

    /* <nr> * (BYTE * 16): Part hashes */
    if( lseek( file, (unsigned)n_part_hashes * 16, SEEK_CUR ) == -1 )
    {
        msg_Err( p_access, "Failed to read part hashes" );
        return 0;
    }

    /* INT32: Number of meta tags */
    if( read( file, &l_meta_tags, sizeof( l_meta_tags ) ) != sizeof( l_meta_tags ) )
    {
        msg_Err( p_access, "Failed to read number of meta tags" );
        return 0;
    }

    l_gap_start = 0;
    l_total_missing = 0;
    sz_filename[0] = '\0';
    sz_fileformat[0] = '\0';
    p_sys->i_media_length = 0;
    p_sys->i_media_bitrate = 0;
    for( l_tag = 0; l_tag < l_meta_tags; l_tag++ )
    {
        uint8_t c_tag_type;
        uint16_t n_name_length;
        char sz_name[4096];
        l_file_pos = lseek( file, 0, SEEK_CUR );

        if( read( file, &c_tag_type, sizeof( c_tag_type ) ) != sizeof( c_tag_type ) )
        {
            msg_Err( p_access, "Failed to read type of tag #%u", l_tag + 1 );
            return 0;
        }

        if( read( file, &n_name_length, sizeof( n_name_length ) ) != sizeof( n_name_length ) )
        {
            msg_Err( p_access, "Failed to read name length of tag #%u", l_tag + 1 );
            return 0;
        }

        if( read( file, sz_name, n_name_length ) != (ssize_t)n_name_length )
        {
            msg_Err( p_access, "Failed to read tag name of tag #%u", l_tag + 1 );
            return 0;
        }

        sz_name[n_name_length] = '\0';

        if ( c_tag_type == TAGTYPE_STRING )
        {
            uint16_t n_value_length;
            char sz_value[4096];

            if( read( file, &n_value_length, sizeof( n_value_length ) ) != sizeof( n_value_length ) )
            {
                msg_Err( p_access, "Failed to read string value length of tag #%u", l_tag + 1 );
                return 0;
            }

            if( read( file, sz_value, n_value_length ) != (ssize_t)n_value_length )
            {
                msg_Err( p_access, "Failed to read string value of tag #%u", l_tag + 1 );
                return 0;
            }

            sz_value[n_value_length] = '\0';

            if( n_name_length == 1 )
            {
                switch( (unsigned char)sz_name[0] )
                {
                    case FT_FILENAME:
                        // If filename contains foreign characters, it is stored twice in the part.met file,
                        // one coded in UTF8 and the other coded in the local codepage.
                        // Just read the first filename tag.
                        if ( sz_filename[0] == '\0' )
                        {
                            strncpy( sz_filename, sz_value, sizeof( sz_filename ) );
                            sz_filename[sizeof( sz_filename ) - 1] = '\0';
                        }
                        break;
                    case FT_FILEFORMAT:
                        strncpy( sz_fileformat, sz_value, sizeof( sz_fileformat ) );
                        sz_fileformat[sizeof( sz_fileformat ) - 1] = '\0';
                        break;
                    case FT_MEDIA_ARTIST:
                        psz_media_artist = _strdup( sz_value );
                        break;
                    case FT_MEDIA_ALBUM:
                        psz_media_album = _strdup( sz_value );
                        break;
                    case FT_MEDIA_TITLE:
                        psz_media_title = _strdup( sz_value );
                        break;
                    default:
                        break;
                }
            }
        }
        else if( c_tag_type == TAGTYPE_UINT32 )
        {
            uint32_t n_value;

            if( read( file, &n_value, sizeof( n_value ) ) != sizeof( n_value ) )
            {
                msg_Err( p_access, "Failed to read integer value of tag #%u", l_tag + 1 );
                return 0;
            }

            if( n_name_length == 1 )
            {
                switch( (unsigned char)sz_name[0] )
                {
                    case FT_FILESIZE:
                        p_sys->l_part_file_size = n_value;
                        break;
                    case FT_MEDIA_LENGTH:
                        p_sys->i_media_length = n_value;
                        break;
                    case FT_MEDIA_BITRATE:
                        p_sys->i_media_bitrate = n_value;
                        break;
                }
            }
            else
            {
                // gap_start = first missing byte, gap_end = first non-missing byte
                if( sz_name[0] == FT_GAPSTART )
                {
                    l_gap_start = n_value;
                    p_sys->p_gaps = (GAP*)realloc( p_sys->p_gaps, sizeof( *p_sys->p_gaps ) * ( p_sys->i_gaps + 1 ) );
                    p_sys->p_gaps[p_sys->i_gaps].l_start = n_value;
                    p_sys->p_gaps[p_sys->i_gaps].l_end = n_value + 1; /* Just in case */
                }
                else if( sz_name[0] == FT_GAPEND )
                {
                    l_total_missing += n_value - l_gap_start;
                    p_sys->p_gaps[p_sys->i_gaps].l_end = n_value;
                    p_sys->i_gaps++;
                }
            }
        }
        else if( c_tag_type == TAGTYPE_UINT64 )
        {
            int64_t n64_value;

            if( read( file, &n64_value, sizeof( n64_value ) ) != sizeof( n64_value ) )
            {
                msg_Err( p_access, "Failed to read integer value of tag #%u", l_tag + 1 );
                return 0;
            }

            if( n_name_length == 1 )
            {
                switch( (unsigned char)sz_name[0] )
                {
                    case FT_FILESIZE:
                        p_sys->l_part_file_size = n64_value;
                        break;
                }
            }
            else
            {
                // gap_start = first missing byte, gap_end = first non-missing byte
                if( sz_name[0] == FT_GAPSTART )
                {
                    l_gap_start = n64_value;
                    p_sys->p_gaps = (GAP*)realloc( p_sys->p_gaps, sizeof( *p_sys->p_gaps ) * ( p_sys->i_gaps + 1 ) );
                    p_sys->p_gaps[p_sys->i_gaps].l_start = n64_value;
                    p_sys->p_gaps[p_sys->i_gaps].l_end = n64_value + 1; /* Just in case */
                }
                else if( sz_name[0] == FT_GAPEND )
                {
                    l_total_missing += n64_value - l_gap_start;
                    p_sys->p_gaps[p_sys->i_gaps].l_end = n64_value;
                    p_sys->i_gaps++;
                }
            }
        }
        else if( c_tag_type == TAGTYPE_FLOAT32 )
        {
            float f_value;

            if( read( file, &f_value, sizeof( f_value ) ) != sizeof( f_value ) )
            {
                msg_Err( p_access, "Failed to read float value of tag #%u", l_tag + 1 );
                return 0;
            }
        }
        else
        {
            msg_Err( p_access, "Unknown tag type %u at "I64Fd, c_tag_type, l_file_pos );
            return 0;
        }
    }

    p_sys->l_avail_size = p_sys->l_part_file_size - l_total_missing;

    qsort( p_sys->p_gaps, p_sys->i_gaps, sizeof( *p_sys->p_gaps ), CmpGaps );

    l_last_end = 0;
    for( i = 0; i < p_sys->i_gaps; i++ )
    {
        if( p_sys->p_gaps[i].l_start > l_last_end )
        {
            // avail_start = first non-missing byte, avail_end = first missing byte
            p_sys->p_avails = (AVAIL*)realloc( p_sys->p_avails, sizeof( *p_sys->p_avails ) * ( p_sys->i_avails + 1 ) );
            p_sys->p_avails[p_sys->i_avails].l_start = l_last_end;
            p_sys->p_avails[p_sys->i_avails].l_end = p_sys->p_gaps[i].l_start;
            p_sys->i_avails++;
        }
        l_last_end = p_sys->p_gaps[i].l_end;
    }

    if( p_sys->l_part_file_size > l_last_end )
    {
        p_sys->p_avails = (AVAIL*)realloc( p_sys->p_avails, sizeof( *p_sys->p_avails ) * ( p_sys->i_avails + 1 ) );
        p_sys->p_avails[p_sys->i_avails].l_start = l_last_end;
        p_sys->p_avails[p_sys->i_avails].l_end = p_sys->l_part_file_size;
        p_sys->i_avails++;
    }

    p_sys->n_file_type = ftNUL;
    psz_format = NULL;

    /* Give the file extension a higher priority when determining the file type.
       Although the information stored in FT_FILEFORMAT should be more accurate,
       the file extension is the only way the user can change/specify the file type!
     */

    if( sz_filename[0] != '\0' )
        psz_format = strrchr( sz_filename, '.' );
    if( !psz_format && sz_fileformat[0] != '\0' )
        psz_format = sz_fileformat;

    if( psz_format )
    {
        /* trim leading spaces and dots */
        while( *psz_format == ' ' || *psz_format == '.' )
            psz_format++;

        if( *psz_format != '\0' )
        {
            char* psz;
            char sz_format[16];

            strncpy( sz_format, psz_format, sizeof( sz_format ) );
            sz_format[sizeof( sz_format ) - 1] = '\0';

            /* trim trailing spaces and dots */
            psz = sz_format + 1;
            while( *psz != '\0' )
            {
                if( *psz == ' ' || *psz == '.' )
                {
                    *psz = '\0';
                    break;
                }
                psz++;
            }

            _strlwr( sz_format );
            if( !strcmp( sz_format, "mpg"  ) ||
                !strcmp( sz_format, "mpeg" ) ||
                !strcmp( sz_format, "mpe"  ) ||
                !strcmp( sz_format, "mpv"  ) ||
                !strcmp( sz_format, "mpv1" ) ||
                !strcmp( sz_format, "mpv2" ) ||
                !strcmp( sz_format, "mpv3" ) ||
                !strcmp( sz_format, "mpv4" ) ||
                !strcmp( sz_format, "mp1v" ) ||
                !strcmp( sz_format, "mp2v" ) ||
                !strcmp( sz_format, "mp3v" ) ||
                !strcmp( sz_format, "mp4v" ) ||
                !strcmp( sz_format, "mpa"  ) ||
                !strcmp( sz_format, "mp1"  ) ||
                !strcmp( sz_format, "mp2"  ) ||
                !strcmp( sz_format, "mp3"  ) ||
                !strcmp( sz_format, "mp4"  ) ||
                !strcmp( sz_format, "mps"  ) )
                p_sys->n_file_type = ftMPG;
            else if( !strcmp( sz_format, "avi" ) )
                p_sys->n_file_type = ftAVI;
        }
    }

    DEBUG_MSG("Partfile size: "I64Fd", Available: "I64Fd, p_sys->l_part_file_size, p_sys->l_avail_size );

    /* Add file details to the Stream and Media Info window in VLC */
    if( sz_filename[0] != '\0' )
    {
        add_p_met_info_str( p_sys->p_input, "Name", (IsUTF8( sz_filename ) ? strdup( sz_filename ) : FromLocaleDup( sz_filename )) );
    }

    if( p_sys->l_part_file_size > 0 )
    {
        char sz_data[64];
        snprintf( sz_data, sizeof sz_data, I64Fd" bytes (%.2f MB)", p_sys->l_part_file_size, p_sys->l_part_file_size / ( 1024.0 * 1024.0 ) );
        add_p_met_info_str( p_sys->p_input, "Size", sz_data );
    }

    if( p_sys->l_avail_size > 0 )
    {
        char sz_data[64];
        snprintf( sz_data, sizeof sz_data, I64Fd" bytes (%.2f MB)", p_sys->l_avail_size, p_sys->l_avail_size / ( 1024.0 * 1024.0 ) );
        add_p_met_info_str( p_sys->p_input, "Available data", sz_data );
    }

    if( sz_fileformat[0] != '\0' )
    {
        add_p_met_info_str( p_sys->p_input, "Format", sz_fileformat );
    }

    if( p_sys->i_media_bitrate > 0 )
    {
        char sz_data[32];
        snprintf( sz_data, sizeof sz_data, "%u kBit/s", p_sys->i_media_bitrate );
        add_p_met_info_str( p_sys->p_input, "Bitrate", sz_data );
    }

    if( p_sys->i_media_length > 0 )
    {
        char sz_data[32];
        SecToTimeLength( p_sys->i_media_length, sz_data, sizeof sz_data );
        add_p_met_info_str( p_sys->p_input, "Length", sz_data );
    }

    if( psz_media_artist )
    {
        add_p_met_info_str( p_sys->p_input, "Artist", psz_media_artist );
    }

    if( psz_media_album )
    {
        add_p_met_info_str( p_sys->p_input, "Album", psz_media_album );
    }

    if( psz_media_title )
    {
        add_p_met_info_str( p_sys->p_input, "Title", psz_media_title );
    }

if (DEBUG_MODE)
{
    char sz_title[16];
    char sz_data[64];
    for( i = 0; i < p_sys->i_gaps; i++ )
    {
        snprintf( sz_title, sizeof sz_title, "GAP %3u", i );
        snprintf( sz_data, sizeof sz_data, I64Fd" - "I64Fd" = "I64Fd"  (%7.2f MB)", p_sys->p_gaps[i].l_start, p_sys->p_gaps[i].l_end - 1,
          p_sys->p_gaps[i].l_end - p_sys->p_gaps[i].l_start, (double)( p_sys->p_gaps[i].l_end - p_sys->p_gaps[i].l_start ) / ( 1024.0 * 1024.0 ) );
        add_p_met_info_str( p_sys->p_input, sz_title, sz_data );
    }

    for( i = 0; i < p_sys->i_avails; i++ )
    {
        snprintf( sz_title, sizeof sz_title, "AVL %3u", i );
        snprintf( sz_data, sizeof sz_data, I64Fd" - "I64Fd" = "I64Fd"  (%7.2f MB)", p_sys->p_avails[i].l_start, p_sys->p_avails[i].l_end - 1,
          p_sys->p_avails[i].l_end - p_sys->p_avails[i].l_start, (double)( p_sys->p_avails[i].l_end - p_sys->p_avails[i].l_start ) / ( 1024.0 * 1024.0 ) );
        add_p_met_info_str( p_sys->p_input, sz_title, sz_data );
    }
}

    vlc_object_release( p_sys->p_input );

    if( psz_media_artist )
        free( psz_media_artist );
    if( psz_media_album )
        free( psz_media_album );
    if( psz_media_title )
        free( psz_media_title );

    DEBUG_MSG("Reading of part.met file completed successfully");
    return 1;
}

/*****************************************************************************
 * Open: open the file
 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;
	char                sz_partmet_file[_MAX_PATH];
	int                 pmet_file;
#ifdef WIN32
    wchar_t wpath[MAX_PATH+1];
    bool is_remote = false;
#endif

    /* Update default_pts to a suitable value for file access */
    var_Create( p_access, "file-caching", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );

    //STANDARD_READ_ACCESS_INIT;
	access_InitFields( p_access );                                      
    ACCESS_SET_CALLBACKS( Read, NULL, Control, Seek );                  
    p_sys = p_access->p_sys = (access_sys_t*)calloc( 1, sizeof( access_sys_t ));       
    if( !p_sys ) 
		return VLC_ENOMEM;

    p_sys->i_nb_reads = 0;
    p_sys->b_pace_control = true;

    DEBUG_MODE = config_GetInt( p_access, "generate_debug_msg" ) != 0;
    p_sys->p_input = (input_thread_t*)vlc_object_find( p_access, VLC_OBJECT_INPUT, FIND_PARENT );

    DEBUG_MSG("Opening part file %s", p_access->psz_path);

	/* Open file */
    int fd = -1;

    if (!_stricmp (p_access->psz_access, "fd"))
        fd = dup (atoi (p_access->psz_path));
    else if (!strcmp (p_access->psz_path, "-"))
        fd = dup (0);
    else
    {
        msg_Dbg (p_access, "opening file `%s'", p_access->psz_path);
        fd = open_file (p_access, p_access->psz_path);
#ifdef WIN32
        if (MultiByteToWideChar (CP_UTF8, 0, p_access->psz_path, -1,
                                 wpath, MAX_PATH)
         && PathIsNetworkPathW (wpath))
            is_remote = true;
# define IsRemote( fd ) ((void)fd, is_remote)
#endif
    }
    if (fd == -1)
        goto error;
	DEBUG_MSG("File opened");
#ifdef HAVE_SYS_STAT_H
    struct stat st;
	DEBUG_MSG("check0 - fd: %u", fd);
    if (fstat (fd, &st))
    {
        msg_Err (p_access, "failed to read (%m)");
        goto error;
    }
    /* Directories can be opened and read from, but only readdir() knows
     * how to parse the data. The directory plugin will do it. */
    if (S_ISDIR (st.st_mode))
    {
        msg_Dbg (p_access, "ignoring directory");
        goto error;
    }
    if (S_ISREG (st.st_mode))
        p_access->info.i_size = st.st_size;
    else if (!S_ISBLK (st.st_mode))
    {
        p_access->pf_seek = NoSeek;
        p_sys->b_pace_control = _stricmp (p_access->psz_access, "stream") != 0;
    }
#else
# warning File size not known!
#endif

    p_sys->fd = fd;

  /**********************************************************
   * Handle part files if media type matches the file types
   * selected in user options
   **********************************************************/

    if( config_GetInt( p_access, "mpg_partfile_stream" ) ||
        config_GetInt( p_access, "avi_partfile_stream" ) ||
        config_GetInt( p_access, "oth_partfile_stream" ) )
    {
       char* psz_ext = strrchr( p_access->psz_path, '.' );
        if( psz_ext != NULL && stricmp( psz_ext, ".part" ) == 0 )
        {
            strcpy( sz_partmet_file, p_access->psz_path );
            strcat( sz_partmet_file, ".met" );

            /* Try to read the part.met file */
            DEBUG_MSG("Opening part.met file %s", sz_partmet_file);
            pmet_file = open( sz_partmet_file, O_BINARY | O_NONBLOCK );
            if( pmet_file != -1 )
            {
                if( !read_part_met_file( p_access, p_sys, pmet_file ) ||
                  ( p_sys->n_file_type == ftAVI && !config_GetInt( p_access, "avi_partfile_stream" ) ) ||
                  ( p_sys->n_file_type == ftMPG && !config_GetInt( p_access, "mpg_partfile_stream" ) ) ||
                  ( p_sys->n_file_type == ftNUL && !config_GetInt( p_access, "oth_partfile_stream" ) ) )
                {
                    /*
                      If reading the part.met file failed, or the plugin is not configured to handle
                      the partfile's media type, exit and let the standard VLC file access module run.
                     */
                    close( pmet_file );
                    msg_Err( p_access, "Partfile access module will not handle this file, aborting...");
                    goto error;
                }
                close( pmet_file );
            }
            else
            {
                msg_Err( p_access, "Failed to open part.met file `%s'", sz_partmet_file );
            }
        }
    }

	if( !p_sys->p_gaps )
		goto error;

	p_sys->l_currentgap = -1;
	p_sys->l_jumps = 0;
	/* Offering the part file stream as a seekable stream does not always work.
	Sometimes there are strange results. Sometimes there is no video shown at all,
	though the audio is played...
	*/
	if( ( p_sys->n_file_type == ftAVI && config_GetInt( p_access, "avi_partfile_seekable" ) ) ||
		( p_sys->n_file_type == ftMPG && config_GetInt( p_access, "mpg_partfile_seekable" ) ) ||
		( p_sys->n_file_type == ftNUL && config_GetInt( p_access, "oth_partfile_seekable" ) ) )
	{
		if (p_access->pf_seek == NoSeek)
			msg_Info (p_access, "Handling file as not seekable (stream)");
		else
			msg_Info (p_access, "Handling file as seekable");
	}
	else
	{
		p_access->pf_seek = NoSeek;
		DEBUG_MSG("Handling file as not seekable (settings)");
	}
	msg_Info( p_access, "File contains "I64Fd" bytes (%.2f MB) of actual data", p_sys->l_avail_size, (float)p_sys->l_avail_size / ( 1024 * 1024 ));


	msg_Info( p_access, "File is handled as part file...");

    return VLC_SUCCESS;

error:
	if (fd != -1)
		close (fd);
	if( p_sys->p_gaps )
		free( p_sys->p_gaps );
	if( p_sys->p_avails )
		free( p_sys->p_avails );
	free( p_sys );
    return VLC_EGENERIC;
}

/*****************************************************************************
 * Close: close the target
 *****************************************************************************/
static void Close (vlc_object_t * p_this)
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;

    DEBUG_MSG( "closing file" );
    close( p_sys->fd );

    if( p_sys->p_gaps )
        free( p_sys->p_gaps );
    if( p_sys->p_avails )
        free( p_sys->p_avails );
    free( p_sys );
}


#include <vlc_network.h>

/*****************************************************************************
 * Read: standard read on a file descriptor.
 *****************************************************************************/
static ssize_t Read( access_t *p_access, uint8_t *p_buffer, size_t i_len )
{
    access_sys_t *p_sys = p_access->p_sys;
    int fd = p_sys->fd;
    ssize_t i_ret;

#ifndef WIN32
    if (p_access->pf_seek == NoSeek)
        i_ret = net_Read (p_access, fd, NULL, p_buffer, i_len, false);
    else
#endif
        i_ret = read (fd, p_buffer, i_len);

    if( i_ret < 0 )
    {
        switch (errno)
        {
            case EINTR:
            case EAGAIN:
                break;

            default:
                msg_Err (p_access, "failed to read (%m)");
                dialog_Fatal (p_access, _("File reading failed"), "%s",
                              _("VLC could not read the file."));
                p_access->info.b_eof = true;
                return 0;
        }
    }
	else if( i_ret > 0 )
	{
		p_access->info.i_pos += i_ret;
		p_sys->l_real_pos += i_ret;
		//DEBUG_MSG( "file position advanced to VLC: "I64Fd" Real: "I64Fd, p_access->info.i_pos, p_sys->l_real_pos );

		bool in_gap = false;
		//bool really_ingap = false;
		int i;
		vlc_value_t seek_pos;

		for (i = 0; i < p_sys->i_gaps; i++ )
		{
			if( p_sys->l_real_pos > (p_sys->p_gaps[i].l_start - ((p_sys->p_gaps[i].l_start >= READ_GRACESKIP) ? READ_GRACESKIP : 0)) && p_sys->l_real_pos < p_sys->p_gaps[i].l_end )
			{
				in_gap = true;
				//DEBUG_MSG( "VLCPos:"I64Fd" RealPos:"I64Fd" GapStart:"I64Fd" GapEnd:"I64Fd, p_access->info.i_pos,
				//	p_sys->l_real_pos, p_sys->p_gaps[i].l_start, p_sys->p_gaps[i].l_end - 1 );
				break;
			}
		}

		if ( p_sys->p_gaps[i].l_end == p_sys->l_part_file_size )
		{
			DEBUG_MSG( "Last chunk is empty, reading allowed till EOF" );
			in_gap = false;
		}

		if ( in_gap )
		{
			// Concatenate available parts if file is unseekable, or is in MPEG format
			if ( p_access->pf_seek == NoSeek || ( p_sys->n_file_type == ftMPG ) )  
			{
				p_sys->l_real_pos = p_sys->p_gaps[i].l_end;
				lseek( p_sys->fd, p_sys->l_real_pos, SEEK_SET );
				DEBUG_MSG( "Seek to end of gap: "I64Fd, p_sys->p_gaps[i].l_end );
			}
			else
			{
				bool dbgFromMeta = false;
				seek_pos.i_time = var_GetTime( p_sys->p_input, "length" );
				mtime_t cur_time = var_GetTime( p_sys->p_input, "time" );

				if ( seek_pos.i_time == 0 )
				{
					seek_pos.i_time = p_sys->i_media_length * 1000000;
					dbgFromMeta = true;
				}
				if ( seek_pos.i_time != 0 )
				{	
					mtime_t target_time = 0;
					target_time = seek_pos.i_time * (double)(p_sys->p_gaps[i].l_end + READ_GRACESKIP)  / p_sys->l_part_file_size;
					
					if (p_sys->l_currentgap == i)
					{
						if ((p_sys->l_real_pos - i_ret) < p_sys->p_gaps[i].l_end)
						{
							int64_t lMissed = p_sys->p_gaps[i].l_end - (p_sys->l_real_pos - i_ret);
							mtime_t tCorrecting = (double)((double)lMissed  / p_sys->l_part_file_size) * seek_pos.i_time;

							if (p_sys->l_jumps == 1 && target_time + 2000000 <= cur_time)
							{
								target_time += tCorrecting;
								DEBUG_MSG( "Frist Correcting jump pos from "I64Fd" to "I64Fd" (%u secs, + %u secs) to correct %0.2f missed MB, current time"I64Fd, target_time - tCorrecting, target_time ,(int)((target_time +tCorrecting) / 1000000), (int)(tCorrecting / 1000000), (float)lMissed/(1024*1024), cur_time);
								p_sys->l_lastcorrectionjump = target_time; 
								p_sys->l_jumps++;
								var_SetTime( p_sys->p_input, "time", target_time );

							}
							else if (p_sys->l_jumps == 2 && p_sys->l_lastcorrectionjump + 200000 <= cur_time)
							{
								tCorrecting *= 1.5f;
								target_time = p_sys->l_lastcorrectionjump + tCorrecting;
								DEBUG_MSG( "Second Correcting jump pos from "I64Fd" to "I64Fd" (%u secs, + %u secs) to correct %0.2f missed MB, current time"I64Fd, target_time - tCorrecting, target_time ,(int)((target_time +tCorrecting) / 1000000), (int)(tCorrecting / 1000000), (float)lMissed/(1024*1024), cur_time);
								p_sys->l_jumps++;
								var_SetTime( p_sys->p_input, "time", target_time );
							}
						}
					}
					else
					{
						p_sys->l_currentgap = i;
						p_sys->l_jumps = 1;
						DEBUG_MSG( "Seek to end of gap (approximate): "I64Fd" Timeset: "I64Fd" (%u secs), Length from Metadata: %s", p_sys->p_gaps[i].l_end, target_time, (int)(target_time / 1000000), (dbgFromMeta ? "Yes" : "No"));
						var_SetTime( p_sys->p_input, "time", target_time );
						Sleep(100);
					}
				}
				else
				{
					seek_pos.f_float = (double)p_sys->p_gaps[i].l_end / p_sys->l_part_file_size;
					var_SetFloat( p_sys->p_input, "position", seek_pos.f_float );
					DEBUG_MSG( "Seek to end of gap (approximate): "I64Fd" Posset: %f", p_sys->p_gaps[i].l_end, seek_pos.f_float);
				}
			}
		}

	}
	else
		p_access->info.b_eof = true;

    p_sys->i_nb_reads++;

#ifdef HAVE_SYS_STAT_H
    if ((p_access->info.i_size && !(p_sys->i_nb_reads % INPUT_FSTAT_NB_READS))
     || (p_access->info.i_pos > p_access->info.i_size))
    {
        struct stat st;

        if ((fstat (fd, &st) == 0)
         && (p_access->info.i_size != st.st_size))
        {
            p_access->info.i_size = st.st_size;
            p_access->info.i_update |= INPUT_UPDATE_SIZE;
        }
    }
#endif
    return i_ret;
}


/*****************************************************************************
 * Seek: seek to a specific location in a file
 *****************************************************************************/
static int Seek (access_t *p_access, int64_t i_pos)
{
    access_sys_t *p_sys = p_access->p_sys;
    p_access->info.i_pos = i_pos;
    p_access->info.b_eof = false;
/*
	int i;
if (DEBUG_MODE)
{
    //msg_Info( p_access, "function Seek called for location "I64Fd, i_pos );
    for( i = 0; i < p_sys->i_gaps; i++ )
    {
        if( i_pos >= p_sys->p_gaps[i].l_start && i_pos < p_sys->p_gaps[i].l_end )
        {
            DEBUG_MSG( "Pos:"I64Fd" GapStart:"I64Fd" GapEnd:"I64Fd" Seek:"I64Fd,
            p_access->info.i_pos, p_sys->p_gaps[i].l_start, p_sys->p_gaps[i].l_end - 1, i_pos );
            break;
        }
    }
}*/

    lseek (p_access->p_sys->fd, i_pos, SEEK_SET);
	p_sys->l_real_pos = p_access->info.i_pos;

    return VLC_SUCCESS;
}

static int NoSeek (access_t *p_access, int64_t i_pos)
{
    /* assert(0); ?? */
    (void) p_access; (void) i_pos;
    return VLC_EGENERIC;
}

/*****************************************************************************
 * Control:
 *****************************************************************************/
static int Control( access_t *p_access, int i_query, va_list args )
{
    access_sys_t *p_sys = p_access->p_sys;
    bool    *pb_bool;
    int64_t *pi_64;

    switch( i_query )
    {
        /* */
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = (p_access->pf_seek != NoSeek);
            break;

        case ACCESS_CAN_PAUSE:
        case ACCESS_CAN_CONTROL_PACE:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = p_sys->b_pace_control;
            break;

        /* */
        case ACCESS_GET_PTS_DELAY:
            pi_64 = (int64_t*)va_arg( args, int64_t * );
            *pi_64 = var_GetInteger( p_access, "file-caching" ) * INT64_C(1000);
            break;

        /* */
        case ACCESS_SET_PAUSE_STATE:
            /* Nothing to do */
            break;

        case ACCESS_GET_TITLE_INFO:
        case ACCESS_SET_TITLE:
        case ACCESS_SET_SEEKPOINT:
        case ACCESS_SET_PRIVATE_ID_STATE:
        case ACCESS_GET_META:
        case ACCESS_GET_PRIVATE_ID_STATE:
        case ACCESS_GET_CONTENT_TYPE:
            return VLC_EGENERIC;

        default:
            msg_Warn( p_access, "unimplemented query %d in control", i_query );
            return VLC_EGENERIC;

    }
    return VLC_SUCCESS;
}

/*****************************************************************************
 * open_file: Opens a specific file
 *****************************************************************************/
static int convert_path (const char *path, wchar_t* wpath)
{
    if (!MultiByteToWideChar (CP_UTF8, 0, path, -1, wpath, MAX_PATH))
    {
        errno = ENOENT;
        return -1;
    }
    wpath[MAX_PATH] = L'\0';
    return 0;
}

static int open_file (access_t *p_access, const char *path)
{
	int fd = -1;
#if defined(WIN32)
    if (!_stricmp (p_access->psz_access, "file")
      && ('/' == path[0]) && isalpha (path[1])
      && (':' == path[2]) && ('/' == path[3]))
        /* Explorer can open path such as file:/C:/ or file:///C:/
         * hence remove leading / if found */
        path++;

	// utf8_open crashed and i'm too lazy trying to figure out why, so just using its code instead of doing a libcall
	wchar_t wpath[MAX_PATH+1];
	if (convert_path (path, wpath) != 0)
	{
		msg_Err (p_access, "Convert error");
	}
	else
	{
		// _O_BINARY added, cant see how it worked without it given seeing that utf8_open isn't adding any flags
		// and at least our CRT is defaulting to textmode
		fd = _wopen (wpath, O_RDONLY | O_NONBLOCK | _O_BINARY, 0666);
	}
#else
	int fd = utf8_open (path, O_RDONLY | O_NONBLOCK /* O_LARGEFILE*/, 0666);
#endif
    if (fd == -1)
    {
        msg_Err (p_access, "cannot open file %s (%m)", path);
        dialog_Fatal (p_access, _("File reading failed"),
                      _("VLC could not open the file \"%s\"."), path);
        return -1;
    }

#if defined(HAVE_FCNTL)
    /* We'd rather use any available memory for reading ahead
     * than for caching what we've already seen/heard */
# if defined(F_RDAHEAD)
    fcntl (fd, F_RDAHEAD, 1);
# endif
# if defined(F_NOCACHE)
    fcntl (fd, F_NOCACHE, 1);
# endif
#endif

    return fd;
}

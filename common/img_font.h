#ifndef	__IMG_FONT_H_INCLUDED__
#define __IMG_FONT_H_INCLUDED__

//	`freetype-config --cflags` `freetype-config --libs`

//	https://www.freetype.org/freetype2/docs/reference/ft2-index.html
//	https://www.freetype.org/freetype2/docs/tutorial/step2.html
//	https://www.freetype.org/freetype2/docs/tutorial/example1.c
//	https://www.freetype.org/freetype2/docs/tutorial/example2.cpp

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <stdint.h>
#include <string>
//#include <locale>
//#include <codecvt>

class	ImageFont
{
public:
	ImageFont( const char* filename, uint32_t height, bool isFitHeight=true )
	{
		FT_Error			error;
		FT_Size_RequestRec	tReqSize;

		// initialize member var.
		m_nBaseline		= height;
		m_piLibrary		= NULL;
		m_piFace		= NULL;

		error	= FT_Init_FreeType( &m_piLibrary );
		if( 0 != error )
		{
			throw	"ERROR: FT_Init_FreeType()";
		}

		error	= FT_New_Face( m_piLibrary, filename, 0, &m_piFace );
		if( 0 != error )
		{
			throw	"ERROR: FT_New_Face";
		}
		
	    tReqSize.type			= FT_SIZE_REQUEST_TYPE_NOMINAL;
	    tReqSize.width			= 0;
	    tReqSize.height			= (height << 6);
	    tReqSize.horiResolution	= 0;
	    tReqSize.vertResolution	= 0;

		if( isFitHeight )
		{
			//	b. Scaled Global Metrics
			int		yMax	= m_piFace->bbox.yMax;
			int		yMin	= m_piFace->bbox.yMin;

			m_nBaseline		= height * yMax / (yMax - yMin);

			tReqSize.type	= FT_SIZE_REQUEST_TYPE_BBOX;
		    tReqSize.height	= (height << 6);
		}

		error	= FT_Request_Size( m_piFace, &tReqSize );
		if( 0 != error )
		{
			throw	"ERROR: FT_Request_Size()";
		}

/*
		{
			int	l,t,r,b;

			CalcRect( l,t,r,b, "({[_gjpqy" );

			printf(
				"font metrics\n"
				" global.units_per_EM        = %d\n"
				" global.ascender            = %6.3f\n"
				" global.descender           = %6.3f\n"
				" global.height              = %6.3f\n"
				" gloval.bbox.xMax           = %6.3f\n"
				" gloval.bbox.xMin           = %6.3f\n"
				" gloval.bbox.yMax           = %6.3f\n"
				" gloval.bbox.yMin           = %6.3f\n"
				" global.underline_position  = %6.3f\n"
				" global.underline_thickness = %6.3f\n"
				" scaled.ascender            = %6.1f\n"
				" scaled.descender           = %6.1f\n"
				" scaled.height              = %6.1f\n"
				" scaled.max_advance         = %6.1f\n"
				" baseline                   = %d\n"
				" %d --> %d\n",
				m_piFace->units_per_EM,       
				m_piFace->ascender / (double)m_piFace->units_per_EM,
				m_piFace->descender / (double)m_piFace->units_per_EM,
				m_piFace->height / (double)m_piFace->units_per_EM,
				m_piFace->bbox.xMax / (double)m_piFace->units_per_EM,
				m_piFace->bbox.xMin / (double)m_piFace->units_per_EM,
				m_piFace->bbox.yMax / (double)m_piFace->units_per_EM,
				m_piFace->bbox.yMin / (double)m_piFace->units_per_EM,
				m_piFace->underline_position / (double)m_piFace->units_per_EM,
				m_piFace->underline_thickness / (double)m_piFace->units_per_EM,
				m_piFace->size->metrics.ascender / 64.0f,
				m_piFace->size->metrics.descender / 64.0f,
				m_piFace->size->metrics.height / 64.0f,
				m_piFace->size->metrics.max_advance / 64.0f,
				m_nBaseline,
				height,
				b );
		}
//*/
	}
	

	~ImageFont()
	{
		FT_Done_Face( m_piFace );
		FT_Done_FreeType( m_piLibrary );
	}
	
	static	std::u32string	GetUnicode32fromUTF8( const char * str )
	{
		std::string		utf8	= str;
		std::u32string	u32str;

		for( size_t i = 0; i < utf8.size(); i++ )
		{
			uint32_t	c	= (uint32_t)utf8[i];

			if( 0 == (0x80 & c) )
			{
				u32str.push_back( c );
			}
			else if( 0xC0 == (0xE0 & c) )
			{
				if( (i+1) < utf8.size() )
				{
					c	=  (uint32_t)(0x1F & utf8[i+0]) << 6;
					c	|= (uint32_t)(0x3F & utf8[i+1]) << 0;
				}
				else
				{
					c	= '?';
				}

				i	+= 1;
				u32str.push_back( c );
			}
			else if( 0xE0 == (0xF0 & c) )
			{
				if( (i+2) < utf8.size() )
				{
					c	=  (uint32_t)(0x0F & utf8[i+0]) << 12;
					c	|= (uint32_t)(0x3F & utf8[i+1]) << 6;
					c	|= (uint32_t)(0x3F & utf8[i+2]) << 0;
				}
				else
				{
					c	= '?';
				}

				i	+= 2;
				u32str.push_back( c );
			}
			else if( 0xF0 == (0xF8 & c) )
			{
				if( (i+3) < utf8.size() )
				{
					c	=  (uint32_t)(0x07 & utf8[i+0]) << 18;
					c	|= (uint32_t)(0x3F & utf8[i+1]) << 12;
					c	|= (uint32_t)(0x3F & utf8[i+2]) << 6;
					c	|= (uint32_t)(0x3F & utf8[i+3]) << 0;
				}
				else
				{
					c	= '?';
				}
				
				i	+= 3;
				u32str.push_back( c );
			}
		}
		return	u32str;
//		return	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);
	}

	int	CalcRect( int& left, int& top, int& right, int& bottom, const char* str )
	{
		return	CalcRect( left, top, right, bottom, GetUnicode32fromUTF8( str ) );
	}

	int	CalcRect( int& left, int& top, int& right, int& bottom, const std::u32string& u32str )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };
		bool			isFirst	= true;

		left	= 0;
		top		= 0;
		right	= 0;
		bottom	= 0;

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			switch( u32str[i] )
			{
			case '\r':
				break;

			case '\t':
				pen.x	+= m_piFace->size->metrics.max_advance * 4;
				pen.x	-= pen.x % (m_piFace->size->metrics.max_advance * 4);
				break;
				
			case '\n':
				pen.x	= 0;
				pen.y	-= m_piFace->size->metrics.height;
				break;
			
			default:
				FT_Set_Transform( m_piFace, &matrix, &pen );
	
				error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
				if( 0 == error )
				{
					int	l	= slot->bitmap_left;
					int	r	= slot->bitmap_left + slot->bitmap.width;
					int	t	= m_nBaseline - slot->bitmap_top;
					int	b	= m_nBaseline - slot->bitmap_top + slot->bitmap.rows;
	
					if( !isFirst )
					{
						left	= left   < l ? left : l;
						top		= top    < t ? top  : t;
						right	= right  < r ? r : right;
						bottom	= bottom < b ? b : bottom;
					}
					else
					{
						left	= l;
						top		= t;
						right	= r;
						bottom	= b;
						isFirst	= false;
					}
	
					pen.x	+= slot->advance.x;
					pen.y	+= slot->advance.y;
				}
				break;
			}
		}

		return	0;
	}
	
	int DrawTextGRAY( int x, int y, const char* str, uint8_t color, uint8_t * image, int stride, int cx, int cy )
	{
		return	DrawTextGRAY( x, y, GetUnicode32fromUTF8(str), color, image, stride, cx, cy );
	}

	int DrawTextGRAY( int x, int y, const std::u32string& u32str, uint8_t color, uint8_t * image, int stride, int cx, int cy )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			switch( u32str[i] )
			{
			case '\r':
				break;

			case '\t':
				pen.x	+= m_piFace->size->metrics.max_advance * 4;
				pen.x	-= pen.x % (m_piFace->size->metrics.max_advance * 4);
				break;

			case '\n':
				pen.x	= 0;
				pen.y	-= m_piFace->size->metrics.height;
				break;
			
			default:
				FT_Set_Transform( m_piFace, &matrix, &pen );

				error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
				if( 0 == error )
				{
					int	bmp_cy	= slot->bitmap.rows;
					int	pos_y	= y + m_nBaseline - slot->bitmap_top;
					int	rs		= 0 <= pos_y ? 0 : -pos_y;
					int	re		= (pos_y + bmp_cy) <= cy ? bmp_cy : (cy - pos_y);
	
					int	bmp_cx	= slot->bitmap.width;
					int	pos_x	= x + slot->bitmap_left;
					int	cs		= 0 <= pos_x ? 0 : -pos_x;
					int	ce		= (pos_x + bmp_cx) <= cx ? bmp_cx : (cx - pos_x);
	
					for( int r = rs; r < re; r++ )
					{
						uint8_t *	src_line	= &slot->bitmap.buffer[ bmp_cx * r ];
						uint8_t *	dst_line	= &image[ stride * (pos_y + r) + pos_x ];
						int 		c			= cs;
	
						for( ; c < ce; c++ )
						{
							int32_t	a0	= src_line[c+0];
	
							if( 0 < a0 )
							{
								int32_t	d0	= dst_line[c+0];
								a0	+= a0 >> 7;
								dst_line[c+0] = (uint8_t)( d0 + (((color - d0) * a0) >> 8) );
							}
						}
					}
	
					pen.x	+= slot->advance.x;
					pen.y	+= slot->advance.y;
				}
				break;
			}
		}
		
		return	0;
	}

	int DrawTextBGRA( int x, int y, const char* str, uint32_t color, uint8_t * image, int stride, int cx, int cy )
	{
		return	DrawTextBGRA( x, y, GetUnicode32fromUTF8(str), color, image, stride, cx, cy );
	}
	
	int DrawTextBGRA( int x, int y, const std::u32string& u32str, uint32_t color, uint8_t * image, int stride, int cx, int cy )
	{
		FT_Error		error;
		FT_GlyphSlot	slot	= m_piFace->glyph;
		FT_Matrix		matrix	= { 1 << 16, 0, 0, 1 << 16 };
		FT_Vector		pen		= { 0, 0 };
		uint32_t		alpha	= (color >> 24) + (color >> 31);

		for( size_t i = 0; i < u32str.size(); i++ )
		{
			switch( u32str[i] )
			{
			case '\r':
				break;
				
			case '\t':
				pen.x	+= m_piFace->size->metrics.max_advance * 4;
				pen.x	-= pen.x % (m_piFace->size->metrics.max_advance * 4);
				break;

			case '\n':
				pen.x	= 0;
				pen.y	-= m_piFace->size->metrics.height;
				break;

			default:
				FT_Set_Transform( m_piFace, &matrix, &pen );
	
				error	= FT_Load_Char( m_piFace, u32str[i], FT_LOAD_RENDER );
				if( 0 == error )
				{
					int	bmp_cy	= slot->bitmap.rows;
					int	pos_y	= y + m_nBaseline - slot->bitmap_top;
					int	rs		= 0 <= pos_y ? 0 : -pos_y;
					int	re		= (pos_y + bmp_cy) <= cy ? bmp_cy : (cy - pos_y);
	
					int	bmp_cx	= slot->bitmap.width;
					int	pos_x	= x + slot->bitmap_left;
					int	cs		= 0 <= pos_x ? 0 : -pos_x;
					int	ce		= (pos_x + bmp_cx) <= cx ? bmp_cx : (cx - pos_x);
	
					for( int r = rs; r < re; r++ )
					{
						uint8_t *	src_line	= &slot->bitmap.buffer[ bmp_cx * r ];
						uint8_t *	dst_line	= &image[ stride * (pos_y + r) + (pos_x * 4) ];
						int 		c			= cs;
	
						for( ; c < ce; c++ )
						{
							int32_t	a	= src_line[c+0];
							
							if( 0 < a )
							{
								uint8_t*	clr	= (uint8_t*)&color;
								int32_t		d0	= dst_line[c*4+0];
								int32_t		d1	= dst_line[c*4+1];
								int32_t		d2	= dst_line[c*4+2];
								int32_t		d3	= dst_line[c*4+3];
		
								a	+= a >> 7;
								a	*= alpha;
		
								dst_line[c*4+0] = (uint8_t)( ((d0 << 16) + (clr[0] - d0) * a) >> 16 );
								dst_line[c*4+1] = (uint8_t)( ((d1 << 16) + (clr[1] - d1) * a) >> 16 );
								dst_line[c*4+2] = (uint8_t)( ((d2 << 16) + (clr[2] - d2) * a) >> 16 );
								dst_line[c*4+3] = (uint8_t)( ((d3 << 16) + (clr[3] - d3) * a) >> 16 );
							}
						}
					}
	
					pen.x	+= slot->advance.x;
					pen.y	+= slot->advance.y;
				}
				break;
			}
		}
		
		return	0;
	}

protected:
	FT_Library		m_piLibrary;
	FT_Face			m_piFace;
	int				m_nBaseline;
};

#endif	// __IMG_FONT_H_INCLUDED__
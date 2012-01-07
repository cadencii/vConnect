#include "stand.h"

using namespace std;

namespace vconnect
{
    class EncodingConverter
    {
    private:
        iconv_t converter;

    public:
        EncodingConverter( const char *from, const char *to )
        {
            this->converter = iconv_open( to, from );
        }

        ~EncodingConverter()
        {
            iconv_close( this->converter );
        }

        string convert( string source )
        {
            string result;
            char *input = const_cast<char *>( source.c_str() );
            size_t remainingInputBytes = source.size();

            char *buffer = new char[remainingInputBytes + 1];
            char *output = buffer;
            size_t remainingOutputBytes = remainingInputBytes;
            size_t outputBytes = remainingInputBytes;

            while( remainingInputBytes > 0 ){
                char *originalInput = input;
                size_t n = iconv( this->converter, &input, &remainingInputBytes, &output, &remainingOutputBytes );
                if( (n != (size_t) - 1 && remainingInputBytes == 0) || (errno == EINVAL) ){
                    remainingInputBytes = 0;
                    result.append( buffer, 0, outputBytes - remainingOutputBytes );
                }else{
                    switch( errno ){
                        case E2BIG:{
                            result.append( buffer, 0, outputBytes - remainingOutputBytes );
                            output = buffer;
                            remainingOutputBytes = outputBytes;
                            break;
                        }
                        case EILSEQ:{
                            result.append( buffer, 0, outputBytes - remainingOutputBytes );
                            result.append( input, 0, 1 );
                            input++;
                            remainingInputBytes--;
                            output = buffer;
                            remainingOutputBytes = outputBytes;
                            break;
                        }
                        default:{
                            result.append( originalInput );
                            remainingInputBytes = 0;
                            break;
                        }
                    }
                }
            }
            output = buffer;
            remainingOutputBytes = outputBytes;
            if( iconv( this->converter , NULL, NULL, &output, &remainingOutputBytes ) != (size_t) - 1 ){
                result.append( buffer, 0, outputBytes - remainingOutputBytes );
            }
            delete [] buffer;
            return result;
        }

    private:
        EncodingConverter();

        static const char *getCharsetFromCodepage( unsigned int codepage )
        {
            switch( codepage ){
                case 932:{
                    return "Shift_JIS";
                }
                case 51932: {
                    return "euc-jp";
                }
                case 50220: {
                    return "iso-2022-jp";
                }
                case 1208:
                case 1209: {
                    return "UTF-8";
                }
                case 1202:
                case 1203: {
                    return "UTF-16LE";
                }
                case 1200:
                case 1201: {
                    return "UTF-16BE";
                }
                case 1204:
                case 1205: {
                    return "UTF-16";
                }
                case 1234:
                case 1235: {
                    return "UTF-32LE";
                }
                case 1232:
                case 1233: {
                    return "UTF-32BE";
                }
                case 1236:
                case 1237: {
                    return "UTF-32";
                }
            }
            return "";
        }

        static bool isValidConverter( iconv_t converter )
        {
            iconv_t invalid = (iconv_t) - 1;
            return (converter == invalid) ? false : true;
        }

        static bool isValidEncoding( unsigned int codepage )
        {
            // まずUTF-8が有効かどうか
            iconv_t cnv = iconv_open( "UTF-8", "UTF-8" );
            if( false == isValidConverter( cnv ) ){
                iconv_close( cnv );
                return false;
            }
            iconv_close( cnv );

            const char *charset_name = getCharsetFromCodepage( codepage );
            iconv_t cnv2 = iconv_open( "UTF-8", charset_name );
            if( false == isValidConverter( cnv2 ) ){
                iconv_close( cnv2 );
                return false;
            }
            iconv_close( cnv2 );

            iconv_t cnv3 = iconv_open( charset_name, "UTF-8" );
            if( false == isValidConverter( cnv3 ) ){
                iconv_close( cnv3 );
                return false;
            }
            iconv_close( cnv3 );
            return true;
        }
    };
}

#ifndef __EncodingConverter_h__
#define __EncodingConverter_h__

#include <iconv.h>

using namespace std;

namespace vconnect
{
    /**
     * マルチバイト文字列のテキストエンコーディングを変換するためのコンバータ
     */
    class EncodingConverter
    {
    private:
        /**
         * システム内部のマルチバイト文字列が使用しているテキストエンコーディング名
         */
        string internalEncoding;

        /**
         * libiconv のコンバータ
         */
        iconv_t converter;

    public:
        /**
         * 変換元と変換先のエンコーディングを指定し、コンバータを初期化する
         * @param from 変換前のエンコーディング名
         * @param to 変換後のエンコーディング名
         */
        EncodingConverter( string from, string to )
        {
            this->converter = iconv_open( to.c_str(), from.c_str() );
            this->internalEncoding = getInternalEncoding();
        }

        ~EncodingConverter()
        {
            iconv_close( this->converter );
        }

        /**
         * 文字列のテキストエンコーディングを変換する
         * @param source 変換する文字列
         * @return エンコーディングを変換した文字列
         */
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

        /**
         * char の内部のエンコーディングを調べる
         * @return
         */
        static string getInternalEncoding()
        {
            unsigned int result = 0;
            char *localeNameRaw = std::setlocale( LC_CTYPE, "" );
#ifdef __APPLE__
            // Macは決め打ち
            result = 1208;
#else
            if( NULL != localeNameRaw ){
                // localeName = "ja_JP.UTF-8" (openSUSE, g++)
                // localeName = "Japanese_Japan.932" (Windows XP, g++)
                string localeName = localeNameRaw;
                int index = localeName.rfind( "." );
                if( index != string::npos ){
                    string encodingName = localeName.substr( index + 1 );
                    // snum部分が数値に変換できるかどうかを調べる
                    if( encodingName.size() == 0 ){
                        result = 1208;
                    }else{
                        bool allnum = true;
                        for( int i = 0; i < encodingName.size(); i++ ){
                            char c = encodingName[i];
                            if( isdigit( c ) == 0 ){
                                allnum = false;
                                break;
                            }
                        }
                        if( allnum ){
                            int encodingNumber = atoi( encodingName.c_str() );
                            result = encodingNumber;
                        }else{
                            result = 1208;
                        }
                    }
                }else{
                    result = 1208;
                }
            }else{
                result = 1208;
            }
#endif
            return getCharsetFromCodepage( result );
        }

        /**
         * コードページの名称から、読込み時の読込単位(バイト)を調べます
         * @return テキストファイルからの読み込み単位
         */
        static int getBytesPerWord( string encoding ){
            encoding = toLower( encoding );
            if( encoding.compare( "shift_jis" ) == 0 ){
                return 1;
            }else if( encoding.compare( "euc-jp" ) == 0 ){
                return 1;
            }else if( encoding.compare( "iso-2022-jp" ) == 0 ){
                return 1;
            }else if( encoding.compare( "utf-8" ) == 0 ){
                return 1;
            }else if( encoding.compare( "utf-16le" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-16be" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-16" ) == 0 ){
                return 2;
            }else if( encoding.compare( "utf-32le" ) == 0 ){
                return 4;
            }else if( encoding.compare( "utf-32be" ) == 0 ){
                return 4;
            }else if( encoding.compare( "utf-32" ) == 0 ){
                return 4;
            }else{
                return 1;
            }
        }

    private:
        EncodingConverter();

        /**
         * 小文字に変換する
         * @param text 変換元の文字列
         * @return 小文字に変換後の文字列
         */
        static string toLower( string text )
        {
            int length = text.length();
            int i;
            for( i = 0; i < length; i++ ){
                text[i] = tolower( text[i] );
            }
            return text;
        }

        /**
         * コードページ番号から、エンコーディング名を取得する
         * @param codepage 調べるコードページ番号
         * @return エンコーディング名
         */
        static string getCharsetFromCodepage( unsigned int codepage )
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

        /**
         * コードページの名称から、コードページ番号を取得する
         * @param name コードページ名称
         * @return コードページ番号
         */
        static unsigned int getCodepageFromCharset( string sname )
        {
            sname = toLower( sname );
            if( sname.compare( "shift_jis") == 0 ){
                return 932;
            }else if( sname.compare( "euc-jp" ) == 0 ){
                return 51932;
            }else if( sname.compare( "iso-2022-jp" ) == 0 ){
                return 50220;
            }else if( sname.compare( "utf-8" ) == 0 || sname.compare( "utf8" ) == 0 ){
                return 1208;
            }else if( sname.compare( "utf-16le"  ) == 0 || sname.compare( "utf16le" ) == 0 ){
                return 1202;
            }else if( sname.compare( "utf-16be" ) == 0 || sname.compare( "utf16be" ) == 0 ){
                return 1200;
            }else if( sname.compare( "utf-16" ) == 0 || sname.compare( "utf16" ) == 0 ){
                return 1204;
            }else if( sname.compare( "utf-32le" ) == 0 || sname.compare( "utf32le" ) == 0 ){
                return 1234;
            }else if( sname.compare( "utf-32be" ) == 0 || sname.compare( "utf32be" ) == 0 ){
                return 1232;
            }else if( sname.compare( "utf-32" ) == 0 || sname.compare( "utf32" ) == 0 ){
                return 1236;
            }else{
                return 0;
            }
        }

        /**
         * 有効なコンバータかどうかを調べる
         * @param converter 調べる対象のコンバータ
         * @return コンバータが有効であれば true を、そうでなければ false を返す
         */
        static bool isValidConverter( iconv_t converter )
        {
            iconv_t invalid = (iconv_t) - 1;
            return (converter == invalid) ? false : true;
        }

        /**
         * 有効なエンコーディングかどうかを取得する
         * @param codepage エンコーディング番号
         * @return 有効なエンコーディングであれば true を、そうでなければ false を返す
         */
        static bool isValidEncoding( unsigned int codepage )
        {
            // まずUTF-8が有効かどうか
            iconv_t cnv = iconv_open( "UTF-8", "UTF-8" );
            if( false == isValidConverter( cnv ) ){
                iconv_close( cnv );
                return false;
            }
            iconv_close( cnv );

            string charset_name = getCharsetFromCodepage( codepage );
            iconv_t cnv2 = iconv_open( "UTF-8", charset_name.c_str() );
            if( false == isValidConverter( cnv2 ) ){
                iconv_close( cnv2 );
                return false;
            }
            iconv_close( cnv2 );

            iconv_t cnv3 = iconv_open( charset_name.c_str(), "UTF-8" );
            if( false == isValidConverter( cnv3 ) ){
                iconv_close( cnv3 );
                return false;
            }
            iconv_close( cnv3 );
            return true;
        }
    };
}

#endif

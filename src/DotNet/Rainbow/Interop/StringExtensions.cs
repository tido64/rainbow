// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Interop
{
    using System.Diagnostics.Contracts;
    using System.Text;

    public static class StringExtensions
    {
        public static Encoding UTF8 { get; } = new UTF8Encoding(false);

        /// <summary>
        /// UTF-8 encodes all the characters in the specified string into a null-terminated sequence
        /// of bytes.
        /// </summary>
        /// <param name="str">The <see cref="string"/> containing the set of characters to encode.</param>
        /// <returns>A byte array containing the results of encoding the specified set of characters.</returns>
        [Pure]
        public static byte[] GetUTF8Bytes(this string str)
        {
            int length = UTF8.GetByteCount(str);
            byte[] bytes = new byte[length + 1];
            UTF8.GetBytes(str, 0, str.Length, bytes, 0);
            bytes[length] = byte.MinValue;
            return bytes;
        }

        [Pure]
        public static string ToCamelCase(this string str)
        {
            if (string.IsNullOrEmpty(str))
            {
                return str;
            }

            if (str.Length == 1)
            {
                return str.ToLowerInvariant();
            }

            return char.ToLowerInvariant(str[0]) + str.Substring(1);
        }

        [Pure]
        public static string ToPascalCase(this string str)
        {
            if (string.IsNullOrEmpty(str))
            {
                return str;
            }

            if (str.Length == 1)
            {
                return str.ToUpperInvariant();
            }

            return char.ToUpperInvariant(str[0]) + str.Substring(1);
        }

        public static string ToSnakeCase(this string str, StringBuilder sb = null)
        {
            if (string.IsNullOrEmpty(str))
            {
                return str;
            }

            if (str.Length == 1)
            {
                return str.ToLowerInvariant();
            }

            sb = sb ?? new StringBuilder(str.Length * 2);
            sb.Append(char.ToLowerInvariant(str[0]));
            for (int i = 1; i < str.Length; ++i)
            {
                char c = str[i];
                if (char.IsUpper(c))
                {
                    sb.Append('_');
                    sb.Append(char.ToLowerInvariant(c));
                }
                else
                {
                    sb.Append(c);
                }
            }

            return sb.ToString();
        }
    }
}

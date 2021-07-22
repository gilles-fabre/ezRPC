using System;
using System.Runtime.InteropServices;

namespace CSharpLog2Reporter
{
    public class Logger
    {
        [DllImport("Log2Reporter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetLogAddress(char[] ipAddress, short port);
        [DllImport("Log2Reporter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogVText(uint module, byte indent, bool lineBreak, char[] formatP, ArgIterator va_list);
        [DllImport("Log2Reporter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogText(uint module, byte indent, bool lineBreak, char[] textP);
        [DllImport("Log2Reporter.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogData(uint module, byte indent, byte[] dataP, short size);

        public static void SetLogAddress(string ipAddress, short port)
        {
            ipAddress += "\x0";
            SetLogAddress(ipAddress.ToCharArray(), port);
        }
        public static void LogVText(uint module, byte indent, bool lineBreak, string format, __arglist) => throw new Exception("LogVText is not implemented in C#");
        public static void LogText(uint module, byte indent, bool lineBreak, string text)
        {
            text += "\x0";
            LogText(module, indent, lineBreak, text.ToCharArray());
        }
        public static void LogData(uint module, byte indent, byte[] data)
        {
            LogData(module, indent, data, (short)data.Length);
        }
    }
}

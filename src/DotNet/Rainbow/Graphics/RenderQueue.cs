// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Threading;

    using Interop;

    public sealed partial class RenderQueue
    {
        private static RenderQueue sharedInstance;

        private RenderQueue(IntPtr ptr)
        {
            Contract.Requires(ptr != IntPtr.Zero);
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = ptr;
        }

        ~RenderQueue()
        {
            Clear();
        }

        private static IntPtr Instance => sharedInstance.NativeHandle;

        public static int Add(Animation animation, string tag = @"")
            => AddWith(addAnimation, animation, tag);

        public static int Add(Label label, string tag = @"")
            => AddWith(addLabel, label, tag);

        public static int Add(SpriteBatch spriteBatch, string tag = @"")
            => AddWith(addSpritebatch, spriteBatch, tag);

        public static void Clear() => clear(Instance);

        public static void CreateInstance(IntPtr instance)
        {
            Contract.Requires(sharedInstance == null);
            Contract.Ensures(sharedInstance != null);

            Interlocked.CompareExchange(ref sharedInstance, new RenderQueue(instance), null);
        }

        public static void Disable(int index) => Do(disable, index);

        public static void Disable(string tag) => Do(disableTag, tag);

        public static void Enable(int index) => Do(enable, index);

        public static void Enable(string tag) => Do(enableTag, tag);

        public static int Insert(int index, Animation animation, string tag = @"")
            => InsertWith(insertAnimation, index, animation, tag);

        public static int Insert(int index, Label label, string tag = @"")
            => InsertWith(insertLabel, index, label, tag);

        public static int Insert(int index, SpriteBatch spriteBatch, string tag = @"")
            => InsertWith(insertSpritebatch, index, spriteBatch, tag);

        public static void Remove(string tag) => Do(remove, tag);

        public static void RemoveAt(int index) => Do(removeAt, index);

        public static void SetTag(int index, string tag)
        {
            Contract.Requires(index >= 0);
            Contract.Requires(!string.IsNullOrEmpty(tag));

            setTag(Instance, index, tag.GetUTF8Bytes());
        }

        private static int AddWith<T>(MemberFuncPCI add, T obj, string tag)
            where T : NativeObject<T>
        {
            Contract.Requires(obj != null);
            Contract.Requires(tag != null);

            return add(Instance, obj.NativeHandle, tag.GetUTF8Bytes());
        }

        private static void Do(MemberActionC func, string tag)
        {
            Contract.Requires(tag != null);

            func(Instance, tag.GetUTF8Bytes());
        }

        private static void Do(MemberActionI func, int index)
        {
            Contract.Requires(index >= 0);

            func(Instance, index);
        }

        private static int InsertWith<T>(MemberFuncIPCI insert, int index, T obj, string tag)
            where T : NativeObject<T>
        {
            Contract.Requires(index >= 0);
            Contract.Requires(obj != null);
            Contract.Requires(tag != null);

            return insert(Instance, index, obj.NativeHandle, tag.GetUTF8Bytes());
        }
    }
}

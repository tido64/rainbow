// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Threading;

    using Interop;

    public sealed class RenderQueue : NativeObject
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

        protected override Action<IntPtr> Delete => (_) => { };

        private static IntPtr Instance => sharedInstance.NativeHandle;

        public static int Add(Animation animation, string tag = @"")
            => AddWith(Core.RenderQueueAddAnimation, animation, tag);

        public static int Add(Label label, string tag = @"")
            => AddWith(Core.RenderQueueAddLabel, label, tag);

        public static int Add(SpriteBatch spriteBatch, string tag = @"")
            => AddWith(Core.RenderQueueAddSpriteBatch, spriteBatch, tag);

        public static void Clear() => Core.RenderQueueClear(Instance);

        public static void CreateSharedInstance(IntPtr instance)
        {
            Contract.Requires(sharedInstance == null);
            Contract.Ensures(sharedInstance != null);

            Interlocked.CompareExchange(ref sharedInstance, new RenderQueue(instance), null);
        }

        public static void Disable(int index) => Invoke(Core.RenderQueueDisable, index);

        public static void Disable(string tag) => Invoke(Core.RenderQueueDisableTag, tag);

        public static void Enable(int index) => Invoke(Core.RenderQueueEnable, index);

        public static void Enable(string tag) => Invoke(Core.RenderQueueEnableTag, tag);

        public static int Insert(int index, Animation animation, string tag = @"")
            => InsertWith(Core.RenderQueueInsertAnimation, index, animation, tag);

        public static int Insert(int index, Label label, string tag = @"")
            => InsertWith(Core.RenderQueueInsertLabel, index, label, tag);

        public static int Insert(int index, SpriteBatch spriteBatch, string tag = @"")
            => InsertWith(Core.RenderQueueInsertSpriteBatch, index, spriteBatch, tag);

        public static void Remove(string tag) => Invoke(Core.RenderQueueRemove, tag);

        public static void RemoveAt(int index) => Invoke(Core.RenderQueueRemoveAt, index);

        public static void SetTag(int index, string tag)
        {
            Contract.Requires(index >= 0);
            Contract.Requires(!string.IsNullOrEmpty(tag));

            Core.RenderQueueSetTag(Instance, index, tag);
        }

        private static int AddWith<T>(Func<IntPtr, IntPtr, string, int> add, T obj, string tag)
            where T : NativeObject
        {
            Contract.Requires(obj != null);
            Contract.Requires(tag != null);

            return add(Instance, obj.NativeHandle, tag);
        }

        private static int InsertWith<T>(
            Func<IntPtr, int, IntPtr, string, int> insert,
            int index,
            T obj,
            string tag)
            where T : NativeObject
        {
            Contract.Requires(index >= 0);
            Contract.Requires(obj != null);
            Contract.Requires(tag != null);

            return insert(Instance, index, obj.NativeHandle, tag);
        }

        private static void Invoke(Action<IntPtr, string> func, string tag)
        {
            Contract.Requires(tag != null);

            func(Instance, tag);
        }

        private static void Invoke(Action<IntPtr, int> func, int index)
        {
            Contract.Requires(index >= 0);

            func(Instance, index);
        }
    }
}

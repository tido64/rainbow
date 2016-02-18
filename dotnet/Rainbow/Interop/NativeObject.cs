// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Interop
{
    using System;
    using System.Diagnostics.Contracts;

    public abstract class NativeObject : IDisposable
    {
        ~NativeObject()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(disposing: false);
        }

        /// <summary>
        /// Handle for the unmanaged object on which all operations are done.
        /// </summary>
        public IntPtr NativeHandle { get; protected set; } = IntPtr.Zero;

        /// <summary>
        /// Delegate for deleting the unmanaged instance handle.
        /// </summary>
        protected abstract Action<IntPtr> Delete { get; }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        protected void SetValue<T>(ref T field, T value, Action<IntPtr, IntPtr> setValue)
            where T : NativeObject
        {
            Contract.Requires(value != null);
            Contract.Ensures(field == value);

            if (field == value)
            {
                return;
            }

            field = value;
            setValue(NativeHandle, value.NativeHandle);
        }

        /// <summary>
        /// Called when managed state (managed objects) should be disposed.
        /// </summary>
        protected virtual void OnDisposing()
        {
        }

        private void Dispose(bool disposing)
        {
            Contract.Ensures(NativeHandle == IntPtr.Zero);

            if (NativeHandle != IntPtr.Zero)
            {
                if (disposing)
                {
                    OnDisposing();
                }

                Delete(NativeHandle);
                NativeHandle = IntPtr.Zero;
            }
        }
    }
}

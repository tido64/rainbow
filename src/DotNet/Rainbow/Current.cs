// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System;
    using System.Collections.Generic;

    public static class Current
    {
        public static int ScreenHeight { get; private set; }

        public static int ScreenWidth { get; private set; }

        private static Dictionary<Type, object> Objects { get; } = new Dictionary<Type, object>();

        public static void ClearRegisteredInstances() => Objects.Clear();

        public static void RegisterInstance<T>(T instance) where T : class
            => Objects[typeof(T)] = instance;

        public static T Resolve<T>() where T : class
        {
            object value;
            return Objects.TryGetValue(typeof(T), out value) ? (T)value : default(T);
        }

        public static void SetScreenSize(int width, int height)
        {
            ScreenWidth = width;
            ScreenHeight = height;
        }
    }
}

// This file is used by Code Analysis to maintain SuppressMessage
// attributes that are applied to this project.
// Project-level suppressions either have no target or are given
// a specific target and scoped to a namespace, type, member, etc.

using System.Diagnostics.CodeAnalysis;

[assembly: SuppressMessage(
    "Potential Code Quality Issues",
    "RECS0018:Comparison of floating point numbers with equality operator",
    Justification = "This property returns true when this SizeF structure has both a width and height of zero; otherwise, false.",
    Scope = "member",
    Target = "~P:Rainbow.Drawing.SizeF.IsEmpty")]
[assembly: SuppressMessage(
    "Potential Code Quality Issues",
    "RECS0025:Non-readonly field referenced in 'GetHashCode()'",
    Justification = "Copied from CoreFX/src/System.Runtime.WindowsRuntime/src/System/Windows/UI/Color.cs",
    Scope = "member",
    Target = "~M:Rainbow.Graphics.Color.GetHashCode~System.Int32")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0106:Finds calls to ToString() which would be generated automatically by the compiler",
    Justification = "False positive; causes error CS1503: Argument 1: cannot convert from 'System.Text.StringBuilder' to 'char'",
    Scope = "member",
    Target = "~M:Rainbow.Program.Main")]
[assembly: SuppressMessage(
    "Potential Code Quality Issues",
    "RECS0108:Warns about static fields in generic types",
    Justification = "By design; Delete should be unique for each type.",
    Scope = "member",
    Target = "~P:Rainbow.Interop.NativeObject`1.Delete")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0113:Redundant comma in array initializer",
    Justification = "Not redundant.")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0129:'internal' modifier is redundant",
    Justification = "Not redundant.")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0145:'private' modifier is redundant",
    Justification = "Not redundant.")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "Suppresses 'await' warnings.",
    Scope = "member",
    Target = "~M:Rainbow.Threading.TaskExtensions.Forget(System.Threading.Tasks.Task)")]

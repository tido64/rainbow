// This file is used by Code Analysis to maintain SuppressMessage
// attributes that are applied to this project.
// Project-level suppressions either have no target or are given
// a specific target and scoped to a namespace, type, member, etc.

using System.Diagnostics.CodeAnalysis;

// Refactoring Essentials (http://vsrefactoringessentials.com/)
[assembly: SuppressMessage(
    "Potential Code Quality Issues",
    "RECS0025:Non-readonly field referenced in 'GetHashCode()'",
    Justification = "Copied from CoreFX/src/System.Runtime.WindowsRuntime/src/System/Windows/UI/Color.cs",
    Scope = "member",
    Target = "~M:Rainbow.Graphics.Color.GetHashCode~System.Int32")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0070:Redundant explicit argument name specification",
    Justification = "Not redundant.")]
[assembly: SuppressMessage(
    "Redundancies in Code",
    "RECS0106:Finds calls to ToString() which would be generated automatically by the compiler",
    Justification = "False positive; causes error CS1503: Argument 1: cannot convert from 'System.Text.StringBuilder' to 'char'",
    Scope = "member",
    Target = "~M:Rainbow.Program.Main")]
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
    Justification = "False positive; referred to by SizeParamIndex in MarshalAsAttribute.",
    Scope = "member",
    Target = "~M:Rainbow.Input.InputManager.OnPointerBegan(Rainbow.Input.Pointer[],System.UInt32)")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "False positive; referred to by SizeParamIndex in MarshalAsAttribute.",
    Scope = "member",
    Target = "~M:Rainbow.Input.InputManager.OnPointerCanceled(Rainbow.Input.Pointer[],System.UInt32)")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "False positive; referred to by SizeParamIndex in MarshalAsAttribute.",
    Scope = "member",
    Target = "~M:Rainbow.Input.InputManager.OnPointerEnded(Rainbow.Input.Pointer[],System.UInt32)")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "False positive; referred to by SizeParamIndex in MarshalAsAttribute.",
    Scope = "member",
    Target = "~M:Rainbow.Input.InputManager.OnPointerMoved(Rainbow.Input.Pointer[],System.UInt32)")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "Sample implementation.",
    Scope = "member",
    Target = "~M:Rainbow.Program.Update(System.UInt64)")]
[assembly: SuppressMessage(
    "Redundancies in Symbol Declarations",
    "RECS0154:Parameter is never used",
    Justification = "Suppresses 'await' warnings.",
    Scope = "member",
    Target = "~M:Rainbow.Threading.TaskExtensions.Forget(System.Threading.Tasks.Task)")]

// StyleCopAnalyzers (https://stylecop.pdelvo.com/)
[assembly: SuppressMessage(
    "StyleCop.CSharp.SpacingRules",
    "SA1025:CodeMustNotContainMultipleWhitespaceInARow",
    Justification = "Padding to align assignment.",
    Scope = "type",
    Target = "~T:Rainbow.Input.KeyMods")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.ReadabilityRules",
    "SA1101:PrefixLocalCallsWithThis",
    Justification = "Redundant.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1600:ElementsMustBeDocumented",
    Justification = "TODO.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1602:EnumerationItemsMustBeDocumented",
    Justification = "TODO.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1611:ElementParametersMustBeDocumented",
    Justification = "TODO.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1615:ElementReturnValueMustBeDocumented",
    Justification = "TODO.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1623:PropertySummaryDocumentationMustMatchAccessors",
    Justification = "Reviewed.")]
[assembly: SuppressMessage(
    "StyleCop.CSharp.DocumentationRules",
    "SA1636:FileHeaderCopyrightTextMustMatch",
    Justification = "Temporarily disabled due to being unable to suppress when needed.")]

# NASA Operational Simulator for Small Satellites (NOS3) Contributing Guide

So you'd like to contribute to NOS3? 
Below are some guidelines for contributors to follow in addition to a standard [code of conduct](https://www.contributor-covenant.org/version/1/4/code-of-conduct/). 
Contributions come in all shapes and sizes. 
We appreciate your help with documentation, unit tests, framework code, continuous-integration, or simply reporting bugs and improvement ideas. 
We can't promise that we'll accept every suggestion or fix every bug in a timely manner but we'll respond to you as quickly as possible.

* [Code of Conduct](#CodeofConduct)
* [Ways to Contribute](#WaystoContribute)
	* [Discussions and Questions](#DiscussionsandQuestions)
	* [Bug Reports](#BugReports)
		* [Before Reporting a Bug](#BeforeReportingaBug)
		* [Reporting a Bug](#ReportingaBug)
		* [What Happens to my Bug Report?](#WhatHappenstomyBugReport)
	* [New Feature Requests](#NewFeatureRequests)
		* [Before Requesting a New Feature](#BeforeRequestingaNewFeature)
		* [Requesting a New Feature](#RequestingaNewFeature)
		* [What Happens to my Feature Request?](#WhatHappenstomyFeatureRequest)
	* [Pull Requests](#PullRequests)
		* [Before starting your Pull Request](#BeforestartingyourPullRequest)
		* [Creating a Pull Request](#CreatingaPullRequest)
		* [What Happens to My Pull Request?](#WhatHappenstoMyPullRequest)

## <a name='WaystoContribute'></a>Ways to Contribute

### <a name='DiscussionsandQuestions'></a>Discussions and Questions

For discussions, questions, or ideas, [start a new discussion](https://github.com/nasa/nos3/discussions/new) in the cFS repository under the Discussions tab. If you prefer email, you can also [join the cfs community mailing list](README.md#join-the-mailing-list).

### <a name='BugReports'></a>Bug Reports

#### <a name='BeforeReportingaBug'></a>Before Reporting a Bug
Perform a cursory search to see if the bug has already been reported.
If a bug has been reported and the issue is still open, add a comment to the existing issue instead of opening a new one.

#### <a name='ReportingaBug'></a>Reporting a Bug

If you find a bug in our code don't hesitate to report it:

1. Open an issue using the bug report template.
2. Describe the issue.
3. Describe the expected behavior if the bug did not occur.
4. Provide the reproduction steps that someone else can follow to recreate the bug or error on their own.
5. If applicable, add code snippets or references to the software.
6. Provide the system the bug was observed on including the hardware, operating system, and versions.
7. Provide any additional context if applicable.
8. Provide your full name or GitHub username and your company organization if applicable.

#### <a name='WhatHappenstomyBugReport'></a>What Happens to my Bug Report?

1. The NOS3 team will label the issue.
2. A team member will try to reproduce the issue with your provided steps. If the team is able to reproduce the issue, the issue will be left to be implemented by someone.

### <a name='NewFeatureRequests'></a>New Feature Requests

NOS3 has a multitude of users from different fields and backgrounds. We appreciate your ideas for enhancements! 

#### <a name='BeforeRequestingaNewFeature'></a>Before Requesting a New Feature

Perform a cursory search to see if the feature has already been requested. 
If a feature request has been reported and the issue is still open, add a comment to the existing issue instead of opening a new one.

#### <a name='RequestingaNewFeature'></a>Requesting a New Feature

1. Open an issue using the feature request template.
2. Describe the feature.
3. Describe the solution you would like.
4. Describe alternatives you've considered.
5. Provide any additional context if applicable.
6. Provide your full name or GitHub username and your company organization if applicable.

#### <a name='WhatHappenstomyFeatureRequest'></a>What Happens to my Feature Request?

1. The project team will label the issue.
2. The project team will evaluate the feature request, possibly asking you more questions to understand its purpose and any relevant requirements. If the issue is closed, the team will convey their reasoning and suggest an alternative path forward.
3. If the feature request is accepted, it will be marked for implementation.

### <a name='PullRequests'></a>Pull Requests

#### <a name='BeforestartingyourPullRequest'></a>Before starting your Pull Request

Ready to Add Your Code? Follow GitHub's fork-branch-pull request pattern.

1. Fork the relevant component.

2. Find the related issue number or create an associated issue that explains the intent of your new code. 

3. Create a new branch in your fork to work on your fix. We recommend naming your branch `fix-ISSUE_NUMBER-<FIX_SUMMARY>`.

3. Add commits to your branch. For information on commit messages, review [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit/).

#### <a name='CreatingaPullRequest'></a>Creating a Pull Request

We recommend creating your pull-request as a "draft" and to commit early and often so the community can give you feedback at the beginning of the process as opposed to asking you to change hours of hard work at the end.

1. For the title, use the title convention `Fix #XYZ, SHORT_DESCRIPTION`.
2. Describe the contribution. First document which issue number was fixed using the template "Fix #XYZ". Then describe the contribution.
3. Provide what testing was used to confirm the pull request resolves the link issue. If writing new code, also provide the associated coverage unit tests. 
4. Provide the expected behavior changes of the pull request.
5. Provide the system the bug was observed on including the hardware, operating system, and versions.
6. Provide any additional context if applicable.
7. Provide your full name or GitHub username and your company or organization if applicable.

#### <a name='WhatHappenstoMyPullRequest'></a>What Happens to My Pull Request?

1. The NOS3 team will label and evaluate the pull request in the next configuration control board meeting.
2. If the pull request is accepted, it will be merged.

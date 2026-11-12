# Code Review Checklist

- Is the PR linked to one Issue, and does it stay within that Issue's scope?
- Does the implementation satisfy each acceptance criterion?
- Are filesystem paths built with `std::filesystem::path` instead of hand-built slash strings?
- Do tests cover ordinary and relevant boundary cases?
- Are tests registered with CTest so CI actually runs them?
- Are Windows and Ubuntu checks passing? If not, is the failure understood and recorded?
- Is each comment specific, respectful, and actionable?
- Did the author respond to each review comment and push a fix where needed?
- Instructor approval is required before merge.

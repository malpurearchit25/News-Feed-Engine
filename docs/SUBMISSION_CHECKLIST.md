# Final Submission Checklist

Complete this checklist after reviewing the final working tree. Do not commit automatically; review the staged/uncommitted state first.

## Repository Contents

- [ ] `src/` contains `main.cpp`, algorithms, data structures, models, services, and storage files.
- [ ] `data/` contains `users.txt`, `posts.txt`, `relationships.txt`, `likes.txt`, and `comments.txt`.
- [ ] `tests/phase9_tests.cpp` remains present.
- [ ] `README.md` is the final README.
- [ ] All seven required files are present in `docs/`.
- [ ] `.gitignore` excludes local build outputs and `.DS_Store`.
- [ ] No temporary executable, object file, sanitizer output, editor file, or unrelated artifact is ready to commit.

## Build and Test

- [ ] Compile from project root:

  ```bash
  g++ -std=c++17 -Wall -Wextra -pedantic \
      src/main.cpp src/storage/FileManager.cpp \
      -o news_feed
  ```

- [ ] Build completes successfully with zero warnings.
- [ ] Start `./news_feed` and confirm the menu loads.
- [ ] Build and run `tests/phase9_tests.cpp`.
- [ ] Confirm: `Phase 9 direct tests: 79 passed, 0 failed.`
- [ ] Verify a save, restart, and load cycle in a safe test copy if a fresh check is needed.
- [ ] Run `git diff --check` and confirm it reports no whitespace errors.

## Documentation and Demonstration

- [ ] README feature list, formulas, capacities, and complexities match source code.
- [ ] `PROJECT_DOCUMENTATION.md` matches source code.
- [ ] `DEMO_GUIDE.md` menu numbers match `src/main.cpp`.
- [ ] `DSA_EXPLANATION.md` covers only implemented structures and algorithms.
- [ ] `VIVA_PREPARATION.md` has been reviewed by every team member.
- [ ] `PRESENTATION_OUTLINE.md` has been turned into the team's preferred slides if slides are required.
- [ ] Team member names are filled in `TEAM_CONTRIBUTIONS.md` and the allocation reflects actual work.
- [ ] The demo has been rehearsed with recorded post/comment IDs.

## Final Review

- [ ] Git status has been reviewed.
- [ ] No source changes were made only for style during code freeze.
- [ ] No new application functionality or unnecessary DSA was introduced in Phase 10.
- [ ] The team understands the limitations: fixed capacities, text files, session-only notifications/undo, and plain-text prototype passwords.
- [ ] Changes remain uncommitted until the team inspects and approves them.

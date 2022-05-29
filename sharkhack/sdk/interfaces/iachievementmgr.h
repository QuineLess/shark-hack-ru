/*
 * Copyright (c) 2019, x64architecture (kurt@x64architecture.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

class IAchievement {
public:
	virtual int         GetAchievementID() = 0;
	virtual const char* GetName() = 0;
	virtual int         GetFlags() = 0;
	virtual int         GetGoal() = 0;
	virtual int         GetCount() = 0;
	virtual bool        IsAchieved() = 0;
	virtual int         GetPointValue() = 0;
	virtual bool        ShouldSaveWithGame() = 0;
	virtual bool        ShouldHideUntilAchieved() = 0;
	virtual const char* GetIconPath() = 0;
	virtual int         GetDisplayOrder() = 0;
};

class IAchievementMgr {
public:
	virtual IAchievement* GetAchievementByIndex(int index, int nPlayerSlot) = 0;
	virtual IAchievement* GetAchievementByDisplayOrder(int orderIndex, int nPlayerSlot) = 0;
	virtual void*         GetAwardByDisplayOrder(int orderIndex, int nPlayerSlot) = 0;
	virtual IAchievement* GetAchievementByID(int id, int nPlayerSlot) = 0;
	virtual int           GetAchievementCount(bool unk = false) = 0;
	virtual int           GetAchievementTotal() = 0;
	virtual void          AwardAchievement(int nAchievementID, int nPlayerSlot) = 0;
	virtual void          OnMapEvent(const char* pchEventName, int nPlayerSlot) = 0;
	virtual void          SaveGlobalStateIfDirty() = 0;
	virtual bool          HasAchieved(const char* pchName, int nPlayerSlot) = 0;
	virtual bool          GetAchievedDuringCurrentGame(int id) = 0;
	virtual bool          WereCheatsEverOn() = 0;
	virtual void*         GetUIProfileInfo() = 0;
	virtual int           ResetProfileInfo() = 0;
	virtual int           SendWriteProfileEvent() = 0;
	virtual int           SendResetProfileEvent() = 0;
	virtual int           GetWriteProfileResult() = 0;
};


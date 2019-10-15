package com.jimi.mes_server.model.base;

import com.jfinal.plugin.activerecord.Model;
import com.jfinal.plugin.activerecord.IBean;

/**
 * Generated by JFinal, do not modify this file.
 */
@SuppressWarnings({"serial", "unchecked"})
public abstract class BaseSopHistoryDetail<M extends BaseSopHistoryDetail<M>> extends Model<M> implements IBean {

	public M setId(java.lang.Integer id) {
		set("id", id);
		return (M)this;
	}
	
	public java.lang.Integer getId() {
		return getInt("id");
	}

	public M setDetailSiteNumber(java.lang.String detailSiteNumber) {
		set("detail_site_number", detailSiteNumber);
		return (M)this;
	}
	
	public java.lang.String getDetailSiteNumber() {
		return getStr("detail_site_number");
	}

	public M setDetailSiteName(java.lang.String detailSiteName) {
		set("detail_site_name", detailSiteName);
		return (M)this;
	}
	
	public java.lang.String getDetailSiteName() {
		return getStr("detail_site_name");
	}

	public M setDetailLine(java.lang.String detailLine) {
		set("detail_line", detailLine);
		return (M)this;
	}
	
	public java.lang.String getDetailLine() {
		return getStr("detail_line");
	}

	public M setDetailWorkshop(java.lang.String detailWorkshop) {
		set("detail_workshop", detailWorkshop);
		return (M)this;
	}
	
	public java.lang.String getDetailWorkshop() {
		return getStr("detail_workshop");
	}

	public M setDetailFactory(java.lang.String detailFactory) {
		set("detail_factory", detailFactory);
		return (M)this;
	}
	
	public java.lang.String getDetailFactory() {
		return getStr("detail_factory");
	}

	public M setDetailTitle(java.lang.String detailTitle) {
		set("detail_title", detailTitle);
		return (M)this;
	}
	
	public java.lang.String getDetailTitle() {
		return getStr("detail_title");
	}

	public M setDetailContent(java.lang.String detailContent) {
		set("detail_content", detailContent);
		return (M)this;
	}
	
	public java.lang.String getDetailContent() {
		return getStr("detail_content");
	}

	public M setPictureNumber(java.lang.String pictureNumber) {
		set("picture_number", pictureNumber);
		return (M)this;
	}
	
	public java.lang.String getPictureNumber() {
		return getStr("picture_number");
	}

	public M setPicturePath(java.lang.String picturePath) {
		set("picture_path", picturePath);
		return (M)this;
	}
	
	public java.lang.String getPicturePath() {
		return getStr("picture_path");
	}

	public M setHistoryId(java.lang.Integer historyId) {
		set("history_id", historyId);
		return (M)this;
	}
	
	public java.lang.Integer getHistoryId() {
		return getInt("history_id");
	}

}

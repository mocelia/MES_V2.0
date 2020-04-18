package com.jimi.mes_standard_capacity.model.base;

import com.jfinal.plugin.activerecord.Model;
import com.jfinal.plugin.activerecord.IBean;

/**
 * Generated by JFinal, do not modify this file.
 */
@SuppressWarnings({"serial", "unchecked"})
public abstract class BaseStandardCapacity<M extends BaseStandardCapacity<M>> extends Model<M> implements IBean {

	public M setId(java.lang.Integer id) {
		set("id", id);
		return (M)this;
	}
	
	public java.lang.Integer getId() {
		return getInt("id");
	}

	public M setModel(java.lang.String model) {
		set("model", model);
		return (M)this;
	}
	
	public java.lang.String getModel() {
		return getStr("model");
	}

	public M setOutput(java.lang.Integer output) {
		set("output", output);
		return (M)this;
	}
	
	public java.lang.Integer getOutput() {
		return getInt("output");
	}

	public M setTime(java.lang.Integer time) {
		set("time", time);
		return (M)this;
	}
	
	public java.lang.Integer getTime() {
		return getInt("time");
	}

	public M setWorkstation(java.lang.Integer workstation) {
		set("workstation", workstation);
		return (M)this;
	}
	
	public java.lang.Integer getWorkstation() {
		return getInt("workstation");
	}

}
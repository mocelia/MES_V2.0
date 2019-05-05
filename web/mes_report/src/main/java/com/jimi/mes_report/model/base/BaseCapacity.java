package com.jimi.mes_report.model.base;

import com.jfinal.plugin.activerecord.Model;
import com.jfinal.plugin.activerecord.IBean;

/**
 * Generated by JFinal, do not modify this file.
 */
@SuppressWarnings({"serial", "unchecked"})
public abstract class BaseCapacity<M extends BaseCapacity<M>> extends Model<M> implements IBean {

	public M setId(java.lang.Integer id) {
		set("id", id);
		return (M)this;
	}
	
	public java.lang.Integer getId() {
		return getInt("id");
	}

	public M setOrders(java.lang.String orders) {
		set("orders", orders);
		return (M)this;
	}
	
	public java.lang.String getOrders() {
		return getStr("orders");
	}

	public M setType(java.lang.Integer type) {
		set("type", type);
		return (M)this;
	}
	
	public java.lang.Integer getType() {
		return getInt("type");
	}

	public M setActualProduct(java.lang.Integer actualProduct) {
		set("actual_product", actualProduct);
		return (M)this;
	}
	
	public java.lang.Integer getActualProduct() {
		return getInt("actual_product");
	}

	public M setHourlyPlanProduct(java.lang.Integer hourlyPlanProduct) {
		set("hourly_plan_product", hourlyPlanProduct);
		return (M)this;
	}
	
	public java.lang.Integer getHourlyPlanProduct() {
		return getInt("hourly_plan_product");
	}

	public M setHourlyActualProduct(java.lang.Integer hourlyActualProduct) {
		set("hourly_actual_product", hourlyActualProduct);
		return (M)this;
	}
	
	public java.lang.Integer getHourlyActualProduct() {
		return getInt("hourly_actual_product");
	}

}

package com.jimi.mes_server.websocket.entity;

import java.util.HashMap;
import java.util.Map;

/**
 * 各种请求类型的超时时间
 * <br>
 * <b>2019年4月9日</b>
 * @author 几米物联自动化部-洪达浩
 */
public class RequestTypeTimeoutTimeManager {

	private static final long SHOW = 30 * 1000;

	private static final long CANCELSHOW = 10 * 1000;

	private static final long LOGIN = 10 * 1000;

	private static final Map<String, Long> map = new HashMap<>();

	static {
		map.put(RequestType.SHOW, SHOW);
		map.put(RequestType.CANCELSHOW, CANCELSHOW);
		map.put(RequestType.LOGIN, LOGIN);
	}


	public static Map<String, Long> getMap() {
		return map;
	}

}
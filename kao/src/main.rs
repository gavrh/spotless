use axum::{
    http::StatusCode, response::{Html, IntoResponse}, routing::get, Router
};
use askama::Template;
use chrono::{ DateTime, Local };

#[derive(Template)]
#[template(path = "index.html")]
struct IndexTemplate {
    footer: Option<FooterTemplate>
}

struct FooterTemplate {
    time: String
}

async fn root() -> impl IntoResponse {
    (
        StatusCode::OK, 
        Html(
            IndexTemplate{
                footer: Some(FooterTemplate {
                    time: format!("{}", Local::now().format("%b %d, %Y (%H:%M)"))
                })
            }.to_string()
        )
    ).into_response()
}

#[tokio::main]
async fn main() {
    // tracing
    tracing_subscriber::fmt::init();

    let app = Router::new()
        .route("/", get(root));

    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, app).await.unwrap();
}

